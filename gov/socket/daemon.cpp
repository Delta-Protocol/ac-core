#include "daemon.h"
#include "protocol.h"
#include "peer_t.h"
#include <us/gov/signal_handler.h>

typedef us::gov::socket::daemon c;
using namespace us::gov;
using namespace us::gov::socket;
using namespace std;

c::daemon():pool(0) {
}

c::daemon(uint16_t port, int edges):b(port), pool(new ctpl::thread_pool(edges)) {
}

c::~daemon() {
	delete pool;
}

void c::on_finish() {
	b::on_finish();
	{
	unique_lock<mutex> lock(mx);
	ready=true;
	}
	cv.notify_all();
}

void c::run() {
	using namespace std::chrono_literals;
	thread listen(&server::run,this);
	thread_::_this.sleep_for(1s);
	{
	unique_lock<mutex> lock(mx);
	while(!thread_::_this.terminated) {
		clients.read_sockets();
		daemon_timer();
		ready=false;
		cv.wait_for(lock,chrono::seconds(30),[&]{return ready;});  //TODO not every 30 secs but at some safe point in the cycle
		cout << "socket: daemon waked up" << endl;
	}
	}
	listen.join();
}

bool c::receive_and_process(client* scl) {
	assert(pool!=0);
	auto* cl=static_cast<peer_t*>(scl);
	pool->push([&,cl](int id) { process_work(cl); });
	return true;
}

bool c::process_work(peer_t *c) {
	datagram* d=c->complete_datagram();
	if (!d || d->error!=0) {
		cout << "socket: daemon: error recv datagram. clients.remove(fd " << c->sock << ") " << endl;
		if (d) delete d;
		clients.remove(c); 
		return true;
	}
	if (!d->completed()) { 
		cout << "socket: daemon: recv partial datagram. returning to listen pool" << endl;
		return true;
	}
	bool processed=process_work(c,d);
	clients.resume(c);
	return processed;
}

client* c::create_client(int sock) {
	auto p=new peer_t(sock);
	p->parent=this;
	return p;
}

