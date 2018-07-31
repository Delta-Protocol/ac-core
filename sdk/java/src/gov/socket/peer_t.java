#ifndef USGOV_2d8af251450aa79ce1d82cd9d9330072930013905de9945ca29228083ef3eee4
#define USGOV_2d8af251450aa79ce1d82cd9d9330072930013905de9945ca29228083ef3eee4

#include "client.h"

namespace us { namespace gov {
namespace socket {
	using namespace std;
	struct daemon;

	struct peer_t: client {
	typedef client b;

	peer_t();
	peer_t(int sock);
	virtual ~peer_t();
        void process_pong();
	bool is_slow() const; //take a lock before call

        bool ping();

	bool process_work(datagram* d);
	virtual void ready() override;
	void dump(ostream& os) const {
	}
	virtual void dump_all(ostream& os) const override {
		dump(os);
		b::dump_all(os);
	}
	virtual void on_connect() override;
	daemon* parent{0};
	// chrono::microseconds latency;
	};
}
}}

#endif

#include "peer_t.h"
#include "daemon.h"
#include "protocol.h"
#include <us/gov/likely.h>

typedef us::gov::socket::peer_t c;
using namespace us::gov::socket;
using namespace std;
using namespace chrono_literals;

c::peer_t(): b(0) {
}

c::peer_t(int sock): b(sock) {
}

c::~peer_t() {
}

void c::ready() {
    parent->clients.read_sockets();
//	parent->_ready.store(true);
//	parent->cv.notify_all();
}


void c::on_connect() {
	if (parent) parent->incorporate(this);
}

bool c::process_work(datagram* d) { //executed by thread from pool 
     assert(d!=0);
     switch(d->service) {
         case protocol::ping: {
            delete d;
            send(new datagram(protocol::pong,"pong"));
            break;
         }
         case protocol::pong: {
            delete d;
            process_pong();
            break;
         }
         default: return false;
     }
     return true;
}

void c::process_pong() {
/*
    lock_guard<mutex> lock(mx);
    latency=chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now()-sent_ping);
    if (!is_slow()) {
      stage=service;
    }
    else {
        stage=exceed_latency; 
        return;
    }
*/
}


bool c::ping() {
/*
    {
    lock_guard<mutex> lock(mx);
    sent_ping=chrono::steady_clock::now();
    }
*/
    return send(new datagram(protocol::ping,"ping")).empty();
}


bool c::is_slow() const {
//    using namespace std::chrono_literals;
//    if (latency>chrono::steady_clock::duration(300ms)) return true; //TODO parameter
    return false;
}



