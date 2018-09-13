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
/*
void c::ready() {
    parent->clients.read_sockets();
//	parent->_ready.store(true);
//	parent->cv.notify_all();
}
*/
//string c::connect(const string& host, uint16_t port, bool block) {
//    return b::connect(host,port,block);
//}

void c::on_detach() {
//cout << this_thread::get_id() << ": on_detach -spinlock" << endl;
    while(!program::_this.terminated) { 
//            cout << "."; cout.flush();
            this_thread::yield(); //spinlock
            if (!busy.load()) break;
    }
//cout << this_thread::get_id() << ": parent=0 & ::close" << endl;
    parent=0;  
    disconnect();
}


void c::disconnect() {
    if (parent) { //managed client
        parent->detach(this);
    }
    else {
        b::disconnect();
    }
}

bool c::process_work(datagram* d) { //executed by thread from pool 
     assert(d!=0);
     switch(d->service) {
         case protocol::gov_socket_ping: {
            delete d;
            send(new datagram(protocol::gov_socket_pong,"pong"));
            break;
         }
         case protocol::gov_socket_pong: {
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
    return send(new datagram(protocol::gov_socket_ping,"ping")).empty();
}


bool c::is_slow() const {
//    using namespace std::chrono_literals;
//    if (latency>chrono::steady_clock::duration(300ms)) return true; //TODO parameter
    return false;
}



