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

string c::connect(const string& host, uint16_t port, bool block) {
	assert(sock==0); //disconnect();
	lock_guard<mutex> lock(mx);
	string r=init_sock(host, port, block);
	if (!r.empty()) {
        return r;
    }
	addr=host;
    on_connect();
	return "";
}
#include <us/gov/stacktrace.h>
void c::disconnect() {
	lock_guard<mutex> lock(mx);
	if (unlikely(sock==0)) return;
	close(sock);
	sock=0;
    print_stacktrace();
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
    tv.tv_sec = 3; //timeout_seconds;
    tv.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	return "";
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
#endif


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
        if (likely(r.second->completed())) {
cout << "SOCKET: recv datagram " << r.second->service << " " << r.second->service_str() << " of size " << r.second->size() << " bytes. HASH " << r.second->compute_hash() << " from " << addr << endl;
   #ifdef DEBUG
   interceptor(*r.second);
   #endif
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
cout << "SOCKET: sent datagram " << d->service << " " << d->service_str() << " of size " << d->size() << " bytes. HASH " << d->compute_hash() << " to " << addr << endl;
   #ifdef DEBUG
   interceptor(*d);
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
cout << "SOCKET: sent datagram " << d.service << " " << d.service_str() << " of size " << d.size() << " bytes. HASH " << d.compute_hash() << " to " << addr << endl;

   #ifdef DEBUG
   interceptor(d);
   #endif
	}
	return r;
}

void c::dump(ostream& os) const {
	os << "memory address: " << this << "; socket: " << sock << "; inet address: " << addr;
}

