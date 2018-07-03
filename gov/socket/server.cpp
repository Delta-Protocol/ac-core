#include "server.h"
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <cassert>
#include <stdlib.h>
#include <unistd.h>
#include <us/gov/signal_handler.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <us/gov/net.h>

using namespace std;
typedef us::gov::socket::server c;

unique_ptr<us::gov::net::os> c::os=make_unique<us::gov::net::posix>();

c::server():port(0) {
}

c::server(uint16_t p):port(p) {
}

c::~server() {
}

int c::make_socket (uint16_t port) {
	int sock;
	struct sockaddr_in name;
	sock=::socket(PF_INET, SOCK_STREAM, 0); 
	if (sock < 0) {
		cerr << "socket: server: could not create socket" << endl;
		return 0;
	}
	{ int optval = 1; setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval); }

	name.sin_family = AF_INET;
	name.sin_port = htons (port);
	name.sin_addr.s_addr = htonl (INADDR_ANY);
	if (::bind(sock, (struct sockaddr *) &name, sizeof (name)) < 0) {
		cerr << "port " << port << " is busy." << endl;
		return 0;
	}
	return sock;
}


bool c::receive_and_process(client*c) {
	clients.resume(c);
}

#include <sys/types.h>
#include <sys/socket.h>


#include <set>
bool c::banned_throttle(const string& addr) {
    static unordered_set<string> throttle;
    auto x=throttle.find(addr);
    if (x==throttle.end()) {
        throttle.emplace(addr);
        return false;
    }
    else {
        return true;
    }
}

#include <us/gov/likely.h>

void c::run() {
	fd_set read_fd_set;
	int i;
	struct sockaddr_in clientname;
	unsigned int size;
	assert(port);
	sock = make_socket(port);
	if (!sock) {
		cerr << "error making socket" << endl;
		program::_this.finish();
		return;
	}
	if (::listen (sock, 1) < 0) {
		cerr << "error listen" << endl;
		close(sock);
		sock=0;
		program::_this.finish();
		return;
	}
	if (!clients.locli.connect("127.0.0.1",port)) {
		close(sock);
		sock=0;
		cerr << "failed connecting the loopback client" << endl;
		program::_this.finish();
		return;
	}

	// Initialize the set of active sockets.
	int loopback;
	size = sizeof (clientname);
	loopback = ::accept(sock, (struct sockaddr *) &clientname,&size);
	if (loopback < 0) {
		cerr << "error in ::accept" << endl;
		close(sock);
		program::_this.finish();
		return;
	}

	signal_handler::_this.add(this);
	char discard[30]; //loopback recv buffer, up to 30 wake up signals
	while (true) {
		if (thread_::_this.terminated) {
			break;
		}
		FD_ZERO(&read_fd_set);
		FD_SET(sock,&read_fd_set);
		FD_SET(loopback,&read_fd_set);
		vector<int> sl=clients.update();
		for (auto& i:sl) FD_SET(i,&read_fd_set);
		if (::select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
			cerr << "error in select" << endl;
			continue;
		}
		if (FD_ISSET(loopback, &read_fd_set)) {
			int r=os->recv(loopback,&discard,30,0);
		}
		if (FD_ISSET(sock, &read_fd_set)) {
			int nnew;
			size = sizeof (clientname);
			nnew = ::accept(sock, (struct sockaddr *) &clientname,&size);
			if (nnew < 0) {
				cerr << "error in ::accept 2" << endl;
				continue;
			}
			cout << "socket: server: accepted, creating client for fd " << nnew << endl;
			auto cl=create_client(nnew);
            if (unlikely(banned_throttle(cl->addr))) {
                delete cl;
            }
            else {
			    clients.add(cl,false);
            }
		}
		for (int i:sl) { //Service all the sockets with input pending.
			cout << "socket: server: scanning fd " << i << endl;
			if (!FD_ISSET (i, &read_fd_set)) continue;
			cout << "socket: server: fd " << i << " is set" << endl;
			auto c=clients.find(i); //no need lock , this thread is the only that changes size of clients
			if (c==clients.end()) {
				cerr << "data arrived for an unknown fd " << i << endl;
				continue;
			}
			clients.hold(c->second);
			receive_and_process(c->second);
		}
	}
	signal_handler::_this.remove(this);
	close(sock);
	close(loopback);
	sock=0;
	loopback=0;
	clients.locli.disconnect();
}

void c::on_finish() {
	clients.read_sockets();
}

void c::incorporate(client*c) {
	assert(c);
	assert(c->sock!=0);
	clients.add(c);

}

void c::disconnect(client*c) {
	c->disconnect();
}

bool c::clients_t::is_here(client* c) const {
	{
	lock_guard<mutex> lock(mx);
	auto i=find(c->sock);
	if(i!=end()) return true;
	}
	if (wadd.find(c)) return true;
	if (holds.find(c)) return true;
	return false;
}

void c::clients_t::add(client* c,bool wakeupselect) { 
	assert(!is_here(c));
	wadd.add(c); 
	if (wakeupselect) read_sockets();
}
void c::clients_t::remove(client* c) { 
	assert(is_here(c));
	wremove.add(c); 
	read_sockets();
}

void c::clients_t::read_sockets() {
	locli.send('W'); //wake up from select
}

void c::clients_t::hold(client* c) { 
	{
	lock_guard<mutex> lock(mx);
	auto i=find(c->sock);
	assert(i!=end());
	erase(i);
	}
	holds.add(c);
}

void c::clients_t::resume(client* c) { 
	bool b=holds.remove(c);
	if(!b) {
		return;
	}
	{
	lock_guard<mutex> lock(mx);
	emplace(c->sock,c);
	}
	read_sockets();
}

void c::clients_t::import() {
	unordered_set<client*> t;
	{
		lock_guard<mutex> lock(wadd.mx);
		t=wadd;
		wadd.clear();
	}
	{
	lock_guard<mutex> lock(mx);
	for (auto i:t) {
		if (!i) continue;
		assert(find(i->sock)==end());
		//cout << "socket: server: moved cli from wadd to the main container fd " << i->sock << endl;
		emplace(i->sock,i);
		//active_fd_set.set(i->sock);
	}
	}
	for (auto i:t) {
		if (!i) continue;
		i->ready();
	}
}

void c::clients_t::finish() {
	unordered_map<int,client*> copy;
	{
	lock_guard<mutex> lock(wremove.mx);
	copy=wremove;
	wremove.clear();
	}
	for (auto c:copy) {
		iterator i;
		{
		lock_guard<mutex> lock(mx);
	    i=find(c.first);
		}
		if (i==end()) { //it is not in main container
			if(!wadd.remove(c.second)) { //it it in the add waiting list?
				if(!holds.remove(c.second)) { //it it in the holds waiting list?
					cerr << "clients_t::finish: could not locate fd " << c.second->sock << " in any container" << endl;
					assert(false);
					exit(1);
				}
			}
		}
		else {
			lock_guard<mutex> lock(mx);
			erase(i); //no other thread can remove from main container, so i should be still valid
		}
		attic.emplace(c.second,chrono::steady_clock::now());
	}
}

c::clients_t::~clients_t() {
	for (auto i:*this) delete i.second;
}

vector<int> c::clients_t::update() {
	finish();
	import();
	attic.purge();

	vector<int> s;
	lock_guard<mutex> lock(mx);
	s.reserve(size());
	for (auto i=begin(); i!=end();) {
		if (i->second->sock) { //disconnected client?
			s.emplace_back(i->first);
			++i;
		}
		else {
			attic.emplace(i->second,chrono::steady_clock::now());
			i=erase(i);
		}
	}
	return move(s);
}

c::pub_t c::clients_t::active() const { //called by worker threads
	pub_t copy;
	lock_guard<mutex> lock(mx);
	{
		lock_guard<mutex> lock2(holds.mx);
		copy.reserve(size()+holds.size());
		for (auto&i:holds) {
			copy.emplace_back(i);
		}
	}
	for (auto&i:*this) {
		copy.emplace_back(i.second);
	}
	return move(copy);
}

c::clients_t::attic_t::~attic_t() {
    for (auto&i:*this) delete i.first; 
}

void c::clients_t::attic_t::purge() { //delete those clients that terminated long ago, in hope there is no more workers on them
	using namespace std::chrono_literals;
	vector<client*> tmp;
	auto n=chrono::steady_clock::now();
	for (auto&i: *this) {
		if (n-i.second>30s) tmp.push_back(i.first);
	}
	for (auto i:tmp) { 
		erase(find(i)); 
		delete i; 
	}
}

void c::dump(ostream& os) const {
	os << "Hello from socker::server" << endl;
	os << "Listening socket: " << sock << endl;
	os << "Clients: " << endl;
	clients.dump(os);

}

void c::clients_t::dump(ostream& os) const {
	int n=0;
	for (auto& i:active()) {
		os << "client #" << n++ << ": "; i->dump(os); os << endl;
	}
    os << "Total active: " << n << endl;

	os << "*add buffer: " << endl;
	wadd.dump(os);
	os << "*remove buffer: " << endl;
	wremove.dump(os);
	os << "*hold buffer: " << endl;
	holds.dump(os);

}

c::clients_t::rmlist::~rmlist() {
    for (auto i:*this) delete i.second; 
}

void c::clients_t::rmlist::add(client* c) {
	lock_guard<mutex> lock(mx);
	assert(c->sock);
	assert(b::find(c->sock)==end());
	emplace(c->sock,c);
}

bool c::clients_t::rmlist::remove(int fd) { //dont delete
	lock_guard<mutex> lock(mx);
	iterator i=b::find(fd);
	if (i==end()) return false;
	erase(i);
	return true;	
}

bool c::clients_t::rmlist::find(int fd) const {
	lock_guard<mutex> lock(mx);
	return b::find(fd)!=end();
}

void c::clients_t::rmlist::dump(ostream& os) const {
	lock_guard<mutex> lock(mx);	
	os << "Size: " << size() << endl;
	for (auto i:*this)
		{ i.second->dump(os); }
}


c::clients_t::wait::~wait() {
    for (auto i:*this) delete i; 
}

void c::clients_t::wait::add(client* c) {
	lock_guard<mutex> lock(mx);
	emplace(c);
}

bool c::clients_t::wait::remove(client* c) { //dont delete
	lock_guard<mutex> lock(mx);
	iterator i=b::find(c);
	if (i==end()) return false;
	erase(i);
	return true;	
}

bool c::clients_t::wait::find(client* c) const {
	lock_guard<mutex> lock(mx);
	return b::find(c)!=end();
}

void c::clients_t::wait::dump(ostream& os) const {
	lock_guard<mutex> lock(mx);	
	os << "Size: " << size() << endl;
	for (auto i:*this)
		{ i->dump(os); }
}



