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
//#include <us/gov/net.h>

using namespace std;
using namespace us::gov::socket;
typedef us::gov::socket::server c;

//unique_ptr<us::gov::net::os> c::os=make_unique<us::gov::net::posix>();

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

/*
void c::receive_and_process(client*c) {
//	clients.resume(*c);
}
*/

#include <sys/types.h>
#include <sys/socket.h>


#include <set>
bool c::banned_throttle(const string& addr) {
return false;
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

void c::on_finish() {
	clients.read_sockets();
}
void c::attach(client*c, bool wakeupselect) {
	assert(c);
	assert(c->m_sock!=0);
	clients.add(c,wakeupselect);
}

void c::detach(client*c) {
	assert(c);
//	assert(c->sock!=0);
	clients.remove(c);
}
/*
bool c::clients_t::is_here(const client& c) const {
	{
	lock_guard<mutex> lock(mx);
	auto i=find(c.sock);
	if(i!=end()) return true;
	}
	if (wadd.find(c)) return true;
	//if (holds.find(c)) return true;
	return false;
}
*/

void c::clients_t::add(client* c, bool wakeupselect) {
	//if (is_here(*c)) return;
	wadd.add(c);
	if (wakeupselect) read_sockets();
}

void c::clients_t::remove(client* c) {
//	if (!is_here(*c)) return;
	if (wremove.add(c))
    	read_sockets();
}

void c::clients_t::read_sockets() {
	static char w='w';
//	locli.send('W'); //wake up from select
	::write(locli.m_sock, &w, 1);
}
/*
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
*/
void c::clients_t::grow() {
	unordered_set<client*> t;
	{
		lock_guard<mutex> lock(wadd.mx);
		t=wadd;
		wadd.clear();
	}
	{
	lock_guard<mutex> lock(mx);
	for (auto i:t) {
//		if (unlikely(!i)) continue;
		assert(i);
		assert(find(i->m_sock)==end());
        assert(i->m_sock!=0);
		//cout << "socket: server: moved cli from wadd to the main container fd " << i->sock << endl;
		emplace(i->m_sock,i);
		//active_fd_set.set(i->sock);
	}
	}
/*
	for (auto i:t) {
		if (unlikely(!i)) continue;
		i->ready();
	}
*/
    if (!t.empty()) read_sockets();
}

void c::clients_t::shrink() {
//	unordered_map<int,client*> copy;
	unordered_set<client*> copy;
	{
	lock_guard<mutex> lock(wremove.mx);
	copy=wremove;
	wremove.clear();
	}
	lock_guard<mutex> lock(mx);
	for (auto c:copy) {
		iterator i;
//assert(c->sock!=0);
        c->on_detach();
		i=find(c->m_sock);
		if (likely(i!=end())) { //it is not in main container
            erase(i); //no other thread can remove from main container, so i should be still valid
            attic.add(c);
        }
        else {
			if (wadd.remove(c)) {
                attic.add(c);
            }
        }

// { //it it in the add waiting list?
/*
				if(!holds.remove(c.second)) { //it it in the holds waiting list?
					cout << "clients_t::finish: could not locate fd " << c.second->sock << " in any container" << endl;
                    return;
					//assert(false);
					//exit(1);
				}
			}

		}
		else {
*/

//		lock_guard<mutex> lock(mx);
//		}
        //c.second->disconnect();

       
	}
}
#include <thread>
c::clients_t::~clients_t() {
//    grow(); // flush wadd
    {
	lock_guard<mutex> lock(mx);
	for (auto i:*this) wremove.add(i.second); // flush me
    }
    shrink(); // flush wremove
    //cout << "This wait can be improved" << endl;
    //this_thread::sleep_for(2s); //TODO wait till threadpool is full (all workers have finished)
}

vector<int> c::clients_t::update() {
        shrink();
        grow();
        attic.purge();
 
        vector<int> s;
        vector<client*> a;
        
        {
        lock_guard<mutex> lock(mx);
        s.reserve(size());
        a.reserve(size());
        for (auto i=begin(); i!=end();) {
                auto& c=*i->second;
                //if (!c.read_ready()) {
                //    ++i;
                //    continue;
                //}
                if (likely(c.m_sock) ) { //disconnected client?
                        if (!c.m_busy.load()) s.emplace_back(c.m_sock);
                        a.emplace_back(&c);
                        ++i;
                }
                else {
                        c.on_detach();
                        attic.add(i->second);
                        i=erase(i);
                }
        }
        }

        {
        lock_guard<mutex> lock(mx_active);
        active_=a;
        }

       return move(s);
}
/*
vector<client*> c::clients_t::active() const { //called by worker threads
    vector<client*> copy;
	lock_guard<mutex> lock(mx);
	copy.reserve(size());
	for (auto&i:*this) {
		copy.emplace_back(i.second);
	}
	return move(copy);
}
*/

c::clients_t::attic_t::~attic_t() {
//    for (auto&i:*this) delete i.first;
    for (auto&i:*this) delete i;
}

void c::clients_t::attic_t::add(client*cl) {
//    emplace(cl,chrono::steady_clock::now());
    emplace(cl);
//    delete cl;
}
void c::clients_t::attic_t::purge() { //delete those clients that terminated long ago, in hope there is no more workers on them
//    static chrono::duration keep_alive_for{1s}; //rocess_work is supposed to be fast, less than 1sec

//	using namespace std::chrono_literals;
//	vector<client*> tmp;
//	auto n=chrono::steady_clock::now();
	for (auto i=begin(); i!=end(); ) {
        if ((*i)->m_busy.load()) {
            ++i;
            continue;
        }
        else {
            delete *i;
            i=erase(i);
           
        }
//		if (n-i.second>keep_alive_for) tmp.push_back(i.first);
//		tmp.push_back(i.first);
	}

//	for (auto i:tmp) {
//		erase(find(i));
//		delete i;
//	}
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
	//os << "*hold buffer: " << endl;
	//holds.dump(os);

}

c::clients_t::rmlist::~rmlist() {
	lock_guard<mutex> lock(mx);
    assert(empty());
//    for (auto i:*this) delete i.second; 
}

bool c::clients_t::rmlist::add(client* c) {
	if(c->m_sock==0) return false;
	lock_guard<mutex> lock(mx);
//	if(b::find(c->sock)==end());
    emplace(c);
    return true;
}
/*
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
*/
void c::clients_t::rmlist::dump(ostream& os) const {
	lock_guard<mutex> lock(mx);
	os << "Size: " << size() << endl;
	for (auto i:*this) {
		i->dump_all(os);
		os << endl;
	}
}


c::clients_t::wait::~wait() {
	lock_guard<mutex> lock(mx);
    for (auto i:*this) delete i;
}
//#include "peer_t.h"
void c::clients_t::wait::add(client* c) {
	lock_guard<mutex> lock(mx);
  //  assert(static_cast<peer_t*>(c)->parent!=0);
	emplace(c);
}

bool c::clients_t::wait::remove(client* c) { //dont delete
	lock_guard<mutex> lock(mx);
	iterator i=b::find(c);
	if (i==end()) return false;
	erase(i);
	return true;
}

bool c::clients_t::wait::find(const client& c) const {
	lock_guard<mutex> lock(mx);
	return b::find(const_cast<client*>(&c))!=end();
}

void c::clients_t::wait::dump(ostream& os) const {
	lock_guard<mutex> lock(mx);
	os << "Size: " << size() << endl;
	for (auto i:*this) {
		i->dump_all(os);
		os << endl;
	}
}

/*
c::clients_t::hold_t::~hold_t() {
}

void c::clients_t::hold_t::add(const client& c) {
	lock_guard<mutex> lock(mx);
        assert(static_cast<peer_t*>(c)->parent!=0);
	emplace(&c);
}

bool c::clients_t::hold_t::remove(const client& c) {
	lock_guard<mutex> lock(mx);
	iterator i=b::find(&c);
	if (unlikely(i==end())) return false;
	erase(i);
	return true;
}

bool c::clients_t::hold_t::find(const client& c) const {
	lock_guard<mutex> lock(mx);
	return b::find(&c)!=end();
}

void c::clients_t::hold_t::dump(ostream& os) const {
	lock_guard<mutex> lock(mx);
	os << "Size: " << size() << endl;
	for (auto i:*this) {
		i->dump_all(os);
		os << endl;
	}
}
*/

client* c::create_client(int sock) {
	return new client(sock);
}


#include <us/gov/likely.h>

#ifdef SIM
#else
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
	auto r=clients.locli.connect("127.0.0.1",port);
    if (unlikely(!r.empty())) {
		close(sock);
		sock=0;
		cerr << "failed connecting the loopback client. " << r << endl;
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
	char discard[4]; //loopback recv buffer, up to 30 wake up signals
cout << "Starting server" << endl;
	while (true) {
		FD_ZERO(&read_fd_set);
		FD_SET(sock,&read_fd_set);
		FD_SET(loopback,&read_fd_set);
        vector<int> sl=clients.update();
//cout << "select fds: " << endl;
        for (auto& i:sl) {
//cout << i << ", ";
            assert(i!=0);
            FD_SET(i,&read_fd_set); 
        }
//cout << endl;
		if (unlikely(::select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)) {
			cerr << "error in select" << endl;
			continue;
		}
		if (unlikely(thread_::_this.terminated)) break;
		if (unlikely(FD_ISSET(loopback, &read_fd_set))) {
            ::recv(loopback,&discard,4,0);
		}
		if (FD_ISSET(sock, &read_fd_set)) {
			int nnew;
			size = sizeof (clientname);
			nnew = ::accept(sock, (struct sockaddr *) &clientname,&size);
//cout << "NEW connection fd " << nnew << endl;
			if (nnew < 0) {
				cerr << "error in ::accept 2" << endl;
				continue;
			}
//			cout << "socket: server: accepted, creating client for fd " << nnew << endl;
			auto cl=create_client(nnew);
            if (unlikely(banned_throttle(cl->m_addr))) {
                delete cl;
            }
            else {
                attach(cl,false);
			    //clients.add(cl,false);
            }
		}
		for (int i:sl) { //Service all the sockets with input pending.
			//cout << "socket: server: scanning fd " << i << endl;
			if (likely(!FD_ISSET (i, &read_fd_set))) continue;
			//cout << "socket: server: fd " << i << " is set" << endl;
			auto c=clients.find(i); //no need lock , this thread is the only that changes size of clients
			if (unlikely(c==clients.end())) {
				cerr << "data arrived for an unknown fd " << i << endl;
				continue;
			}
			auto p=c->second;
//cout << "Incoming data to fd " << i << endl;

            if (p->m_busy.load()) continue;
            p->m_busy.store(true);
			//clients.hold(p); 
//cout << "receive_and_process fd " << i << endl;
			receive_and_process(p);
//cout << "/receive_and_process fd " << i << endl;
		}
	}
	signal_handler::_this.remove(this);
	close(sock);
	close(loopback);
	sock=0;
	loopback=0;
	clients.locli.disconnect();
}
#endif

