package us.gov.socket;

import java.net.Socket;

	public class client {
        public client() {
//            sock=0;
        }

		client();
		client(Socket sock);
/*
		virtual ~client() {
			disconnect();
			  //you must override destructor call the virtual function disconnect on the most specialized class
		}
*/
		virtual String connect(String host, int port, boolean block=false) {
        	//lock_guard<mutex> lock(mx);
        	String r=init_sock(host, port, block);
        	if (r!=null) {
                return r;
            }
        	addr=host;
            on_connect();
        	return null;
        }

		virtual void disconnect() {
        //	lock_guard<mutex> lock(mx);
        	if (!connected()) return;
        	sock.close();
        }

        inline boolean connected() const { return sock.isConnected(); }

		String address() {
            return sock.getHostAddress().getHostAddress();
        }

		void ready() {}

public class pair<f,s>
    public f first;
    public s second;

    public pair(f fst, s snd) {
        this.first=fst;
        this.second=snd;
     }
 }


pair<String,datagram> recv(short expected_service) {
    pair<String,datagram> r=recv();
    if (r.first!=null) {
        return r;
    }
    datagram d=r.second;
    while(true) { ///delete garbage injected by hackers, discovered when gov trolled wallet with vote mesages
        if (d.service==us::gov::protocol::gov_socket_error) {
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



        pair<String,datagram> send_recv(datagram d,short expected_service);

	    pair<string,datagram*> send_recv(datagram* d);
		string send(datagram* d);
		string send(const datagram& d);


pair<String,datagram> recv() { //caller owns the returning object
    pair<String,datagram> r;
    r.second=new datagram();
    while(true) {
        String ans=r.second.recvfrom(sock); //,socket::response_timeout_secs);
        if (ans==null) {
            r.first=ans;
            r.second=null;
            disconnect();
            break;
        }
        if (r.second.completed()) {
            break;
        }
    }
    return move(r);
}

		void init_sockaddr (struct sockaddr_in *name, const char *hostname, uint16_t port);
		string init_sock(const string& host, uint16_t port, bool block=false);

		virtual void on_connect() {}

		void dump(ostream& os) const;
                virtual void dump_all(ostream& os) const {
                        dump(os);
                }

        Socket sock;
		String msg;
		String addr;
		//mutable mutex mx;
	}


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


c::client(int sock):sock(sock) {
   if (sock!=0) addr=address();
}

string c::address() const {
}

string c::connect(const string& host, uint16_t port, bool block) {
}
#include <us/gov/stacktrace.h>

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
    tv.tv_sec = 3; //timeout_seconds;
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

ofstream _log("/tmp/us_gov_socket_io_log");

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
_log << "SOCKET: " << prefix << " datagram " << d.service << " " << d.service_str() << " of size " << d.size() << " bytes. HASH " << d.compute_hash() << " to " << addr << endl;
_log << "      : " << d.parse_string() << endl;
}

#endif




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
	os << "memory address: " << this << "; socket: " << sock << "; inet address: " << addr;
}

