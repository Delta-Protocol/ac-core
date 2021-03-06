#include "client.h"
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <cassert>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <us/gov/signal_handler.h>
#include <us/gov/likely.h>
#include "protocol.h"


typedef us::gov::socket::client c;
using namespace std;
using namespace us::gov::socket;

c::client():sock(0) {
}

c::client(int sock):sock(sock) {
   if (sock!=0) addr=address();
}

c::~client() {
    disconnect();
}

#ifdef SIM

#else
string c::address() const {
	struct sockaddr_storage addr;
	socklen_t len=sizeof addr;
	int a=getpeername(sock, (struct sockaddr*)&addr, &len);
	if (a!=0) return "";
	char ipstr[len]; // deal with both IPv4 and IPv6
	if (addr.ss_family == AF_INET) {
	    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
	    int port = ntohs(s->sin_port);
	    inet_ntop(AF_INET, &s->sin_addr, ipstr, len); //sizeof ipstr);
	} else { // AF_INET6
	    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
	    int port = ntohs(s->sin6_port);
	    inet_ntop(AF_INET6, &s->sin6_addr, ipstr, len); //sizeof ipstr);
	}
	return ipstr;
}
#endif 

string c::connect(const string& host, uint16_t port, bool block) {
	lock_guard<mutex> lock(mx);
	assert(sock==0); //disconnect();
	string r=init_sock(host, port, block);
	if (!r.empty()) {
        return r;
    }
	addr=host;
	return "";
}

#include <us/gov/stacktrace.h>

void c::disconnect() {
//cout << "disconnect fd " << sock << endl;
	lock_guard<mutex> lock(mx);
	if (unlikely(sock==0)) return;
	::close(sock);
	sock=0; //need the socket id even thout we cut connection
    //print_stacktrace();
}

void c::init_sockaddr (struct sockaddr_in *name, const char *hostname,	uint16_t port) {
	struct hostent *hostinfo;
	name->sin_family = AF_INET;
	name->sin_port = htons (port);
	hostinfo=::gethostbyname(hostname);
	if (hostinfo == 0) {
		cerr << "Unknown host " << hostname << endl;
		exit(EXIT_FAILURE);
	}
	name->sin_addr = *(struct in_addr *) hostinfo->h_addr;
}

#include <fcntl.h>
#include <chrono>
#include <thread>
#include <sstream>

#ifdef SIM
#else
string c::init_sock(const string& host, uint16_t port, bool block) {
	sockaddr_in servername;
	if (unlikely(sock!=0)) {
	  sock=0;
      return "socket is non zero.";
	}
	sock=::socket(PF_INET, SOCK_STREAM, 0);// Create the socket.
	{
	auto flags = fcntl(sock, F_GETFL, 0);
	if (!block) flags|=O_NONBLOCK;
	fcntl(sock,F_SETFL,flags);
	}

	init_sockaddr (&servername, host.c_str(), port);//Connect to the server.
	int r=::connect(sock, (struct sockaddr *) &servername, sizeof (servername));
	if (unlikely(r<0)) {
		if (errno==EINPROGRESS) {
            typedef chrono::high_resolution_clock clock;
            using namespace chrono_literals;

            int error = 0;
            socklen_t len = sizeof (error);
            auto t1 = clock::now();
            struct sockaddr_storage addr;
            socklen_t lena=sizeof addr;
            while(true) {
	            int a=getpeername(sock, (struct sockaddr*)&addr, &lena);
	            if (a==0) break;
	            if (chrono::duration_cast<std::chrono::milliseconds>(clock::now() - t1).count()>500) {
                    return "Error. Timeout obtaining peername";
                }
                this_thread::sleep_for(10ms);
            }
	     return "";
		}
		::close(sock);
		sock=0;
        ostringstream os;
        os << "Connection refused. errno=" << errno;
        return os.str();
	}
    struct timeval tv;
    tv.tv_sec = 3; //0; //timeout_seconds; TODO
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	return "";
}
#endif


pair<string,datagram*> c::send_recv(datagram* d,uint16_t expected_service) {
    pair<string,datagram*> ans;
    ans.first=send(d);
    if (unlikely(!ans.first.empty())) {
        ans.second=0;
        return move(ans);
    }
    return recv(expected_service);
}

pair<string,datagram*> c::send_recv(datagram* d) {
    pair<string,datagram*> ans;
    ans.first=send(d);
    if (unlikely(!ans.first.empty())) {
        ans.second=0;
        return move(ans);
    }
    return recv();
}

#ifdef DEBUG
#include <us/gov/stacktrace.h>
#include <fstream>
#include <chrono>
#include <sstream>

string logfn() {
ostringstream os;
os << "/tmp/us_gov_socket_io_" << getpid();
//cerr << "DEBUG: protocol log at " << os.str() << endl;
return os.str();
}

ofstream _log(logfn());

void interceptor(const datagram& d) {
typedef unordered_set<uint16_t> dt;
static dt svc;
static int i=0;
if (++i%100==0) { //reload file
    i=0;
    svc.clear();
    ifstream f("/tmp/interceptor");

    while (f.good()) {
        uint16_t s;
        f >> s;
        svc.emplace(s);
    }

    if (svc.find(d.service)!=svc.end()) print_stacktrace(); 
}
}

void dump_d(string prefix, const datagram& d, const string& addr) {
_log << chrono::duration_cast<chrono::microseconds>(chrono::system_clock::now().time_since_epoch()).count() << " ";
_log << "SOCKET: " << prefix << " datagram " << d.service << " " << d.service_str() << " of size " << d.size() << " bytes. HASH " << d.compute_hash() << " to " << addr << endl;
_log << "      : " << d.parse_string() << endl;
}

#endif

pair<string,datagram*> c::recv(uint16_t expected_service) { //caller owns the returning object
    auto r=recv();
    if (!r.first.empty()) {
        return move(r);
    }
    datagram*d=r.second;
    while(true) { ///delete garbage injected by hackers, discovered when gov trolled wallet with vote mesages
        if (d->service==us::gov::protocol::gov_socket_error) {
            return move(r);
        }
        if (d->service==expected_service) {
            break;
        }
        else {
            delete d;
        }
        auto r=recv();
        if (!r.first.empty()) {
            return move(r);
        }
        d=r.second;
    }
    return make_pair("",d);
}


pair<string,datagram*> c::recv() { //caller owns the returning object
    pair<string,datagram*> r;
    r.second=new datagram();
    while(!program::_this.terminated) {
        string ans=r.second->recvfrom(sock); //,socket::response_timeout_secs);
        if (unlikely(!ans.empty())) {
            r.first=ans;
	        delete r.second;
            r.second=0;
            disconnect();
            break;
        }
        if (r.second->completed()) {
   #ifdef DEBUG
           dump_d("recv", *r.second, addr); 
   #endif
//   interceptor(*r.second);
            break;
        }
    }
    return move(r);
}

string c::send(datagram* d) { //don't call send(&d) perf
	if (unlikely(!sock)) {
		return "Error. Sending datagram before connecting.";
	}
	assert(d);
	auto r=d->sendto(sock);
    if (unlikely(!r.empty())) {
        disconnect();
    }
    else {

   #ifdef DEBUG
   dump_d("sent", *d, addr); 
//   interceptor(*r.second);
   #endif


    }
    delete d;
    return r;
}

string c::send(const datagram& d) {
	if (unlikely(!sock)) {
		return "Error. Sending datagram before connecting.";
	}
	auto r=d.sendto(sock);
    if (unlikely(!r.empty())) {
        disconnect();
    }
	else {

   #ifdef DEBUG
   dump_d("sent", d, addr); 
//   interceptor(*r.second);
   #endif

	}
	return r;
}

void c::dump(ostream& os) const {
	os << "socket client: mem addr: " << this << " fd: " << sock << "; inet addr: " << addr << endl;
}

