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
#include <gov/signal_handler.h>

typedef usgov::socket::client c;
using namespace std;
using namespace usgov::socket;


c::client():sock(0) {
}
c::client(int sock):sock(sock) {
   if (sock!=0) addr=address();
}

c::~client() {
	disconnect();
}

string client::address() const {
	socklen_t len;
	struct sockaddr_storage addr;
	char ipstr[INET6_ADDRSTRLEN];
	int port;
	len = sizeof addr;
	auto a=getpeername(sock, (struct sockaddr*)&addr, &len);
	if (a!=0) return "";

	// deal with both IPv4 and IPv6:
	if (addr.ss_family == AF_INET) {
	    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
	    port = ntohs(s->sin_port);
	    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
	} else { // AF_INET6
	    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
	    port = ntohs(s->sin6_port);
	    inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
	}
	return ipstr;
}

bool client::connect(const string& host, uint16_t port, bool block) {
	disconnect();
	lock_guard<mutex> lock(mx);
	if (!init_sock(host, port, block)) return false;
	addr=host;
	return true;
}

void client::disconnect() {
	lock_guard<mutex> lock(mx);
	if (sock==0) return;
	close(sock);
	sock=0;
}

void client::init_sockaddr (struct sockaddr_in *name, const char *hostname,	uint16_t port) {
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
bool c::init_sock(const string& host, uint16_t port, bool block) {
	sockaddr_in servername;
	sock=::socket(PF_INET, SOCK_STREAM, 0);// Create the socket.
	if (sock < 0) {
          perror("socket (client)");
	  sock=0;
	  return false;
	}
	{
	auto flags = fcntl(sock, F_GETFL, 0);
	if (!block) flags|=O_NONBLOCK;
	fcntl(sock,F_SETFL,flags);
	}

	init_sockaddr (&servername, host.c_str(), port);//Connect to the server.
	int r=::connect(sock, (struct sockaddr *) &servername, sizeof (servername));
	if (r<0) {
		if (errno==EINPROGRESS) {
			cout << "socket: client: ::connect in progress fd" << sock  << endl;
			return true;
		}
		cout << "socket: client: ::connect failed " << sock << " error: " << r << endl;
		::close(sock);
		sock=0;
		return false;
	}
	return true;
}

//busy-polling all sockets http://www.wangafu.net/~nickm/libevent-book/01_intro.html

bool c::send(int service, const string& payload) {
	datagram* d=new datagram(service,payload);
	if (!send(d)) {
		delete d;
		return false;
	}
	return true;
}

bool c::send(char d) const { 
	if (!sock) {
		cout << "socket: client: cannot send char, sock is 0" << endl;
		return false;
	}
	return io::send(sock,d); 
}

bool c::send(datagram* d) const { 
	if (!sock) {
		cout << "socket: client: cannot send, sock is 0" << endl;
		return false;
	}
	assert(d); 
	return io::send(sock,d); 
}

bool c::send(const datagram& d) const {
	if (!sock) return false;
	return io::send(sock,d); 
}

void c::dump(ostream& os) const {
	os << this << "- file descriptor:" << sock << "; address: " << addr;
}

