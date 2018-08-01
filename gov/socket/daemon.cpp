#include "daemon.h"
#include "protocol.h"
#include "peer_t.h"
#include <us/gov/signal_handler.h>
#include <us/gov/likely.h>

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
}

void c::run() {
	if (pool==0) {
		cerr << "Error. No worker threads available" << endl;
		return;
	}

	using namespace std::chrono_literals;
	thread listen(&server::run,this);
	thread_::_this.sleep_for(1s);
	{
	while(!thread_::_this.terminated) {
		cout << "triggered timer (mutation) v=30 secs" << endl;
		daemon_timer();
		thread_::_this.sleep_for(chrono::seconds(30));  //TODO not every 30 secs but at some safe point in the cycle
	}
	}
	listen.join();
}

void c::dump(ostream& os) const {
    pool->dump(os);
}

void c::receive_and_process(client* scl) {
	assert(pool!=0);
	auto* cl=static_cast<peer_t*>(scl);
    if (pool->n_idle()>0) {
    	pool->push([&,cl](int id) { process_work(cl); });
    }
    else { //drop the connection
        clients.resume(scl);
        scl->disconnect();
    }
}

void c::process_work(peer_t *c) {
	auto r=c->recv(); //complete_datagram();
	if (unlikely(!r.first.empty())) {
//		cerr << r.first << endl; //"socket: daemon: error recv datagram. clients.remove(fd " << c->sock << ") " << endl;
		assert(!r.second);
        c->disconnect();
//		cerr << "peer killed" << endl; //"socket: daemon: error recv datagram. clients.remove(fd " << c->sock << ") " << endl;
		return; //processed work
	}
    assert(r.second!=0);
	if (unlikely(!r.second->completed())) {
        	delete r.second;
		return;
	}
	bool processed=process_work(c,r.second);
    if (!processed) {
        delete r.second;
        c->disconnect();
    }
	clients.resume(c);
}

bool c::process_work(socket::peer_t *c, datagram*d) {
    bool ispong=d->service==protocol::pong;
    if (c->process_work(d)) {
        return true;
    }
    return false;
}

client* c::create_client(int sock) {
	auto p=new peer_t(sock);
	p->parent=this;
	return p;
}

