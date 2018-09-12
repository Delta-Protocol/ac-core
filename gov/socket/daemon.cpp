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

void c::attach(client*c, bool wakeupselect) {
    assert(c!=0);
    assert(static_cast<peer_t*>(c)->parent==0);
    static_cast<peer_t*>(c)->parent=this;
//cout << "attach " << c << " " << static_cast<peer_t*>(c)->parent << endl;
    b::attach(c, wakeupselect);
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
		pool->dump(cout);
		thread_::_this.sleep_for(chrono::seconds(30));  //TODO not every 30 secs but at some safe point in the cycle
	}
	}
	listen.join();
}

void c::dump(ostream& os) const {
    pool->dump(os);
}

void c::receive_and_process(client* c) {
	assert(pool!=0);
	auto* cl=static_cast<peer_t*>(c);

//    process_work(cl);

    if (likely(pool->n_idle()>0)) {
    	pool->push([&,cl](int id) { process_work(cl); });
    }
    else { //drop the connection
//        clients.resume(*scl);
        c->disconnect();
        ++perf.disconnections.thread_pool.full;
    }
}


void c::process_work(peer_t *c) {
//cout << this_thread::get_id() << ": worker takes lock" << endl;
    while(true) {
//        if(unlikely(c->parent==0)) {
//cout << this_thread::get_id() << ": worker releases lock (parent=0)" << endl;
//            return; //client has been detached
//        }


//cout << this_thread::get_id() << ": recv" << endl;
	auto r=c->recv(); //complete_datagram();
//cout << this_thread::get_id() << ": /recv" << endl;
	if (unlikely(!r.first.empty())) {
//		cerr << r.first << endl; //"socket: daemon: error recv datagram. clients.remove(fd " << c->sock << ") " << endl;
		assert(!r.second);
        //assert(c->parent!=0);
        //c->disconnect();
//		cerr << "peer killed" << endl; //"socket: daemon: error recv datagram. clients.remove(fd " << c->sock << ") " << endl;
		break; //processed work
	}
    assert(r.second!=0);
	if (unlikely(!r.second->completed())) {
        delete r.second;
//cout << this_thread::get_id() << ": not completed" << endl;
		break;
	}
//cout << this_thread::get_id() << ": process work" << endl;
	bool processed=process_work(c,r.second);
    if (!processed) {
//cout << this_thread::get_id() << ": not processed" << endl;
        delete r.second;
        c->disconnect();
        ++perf.disconnections.datagram.unknown_service;
    }

    break;
    }

//cout << this_thread::get_id() << ": worker releases lock" << endl;
    c->busy.store(false);
    clients.read_sockets();//c might have more datagrams to process

//    c->idle=true;
//	clients.resume(c);
}

bool c::process_work(socket::peer_t *c, datagram*d) {
//    bool ispong=d->service==protocol::pong;
    if (c->process_work(d)) {
        return true;
    }
    return false;
}

client* c::create_client(int sock) {
	return new peer_t(sock);
//	p->parent=this;
//	return p;
}
