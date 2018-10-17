/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#include "peer_t.h"

#include<thread>

#include "us/gov/likely.h"
#include "protocol.h"
#include "daemon.h"

typedef us::gov::socket::peer_t c;
using namespace us::gov::socket;
using namespace std;
using namespace chrono_literals;

peer_t::peer_t(): client(0) {
}

peer_t::peer_t(int sock): client(sock) {
}

peer_t::~peer_t() {
}

void peer_t::on_detach() {
    while(!program::_this.terminated) { 
        this_thread::yield(); //spinlock
        if (!m_busy.load()) break;
    }
    m_parent=0;
    disconnect();
}

void peer_t::disconnect() {
    if (m_parent) { //managed client
        m_parent->detach(this);
    }
    else {
        client::disconnect();
    }
}

bool peer_t::process_work(datagram* d) { //executed by thread from pool
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

void peer_t::process_pong() {
}

bool peer_t::ping() {
    return send(new datagram(protocol::gov_socket_ping,"ping")).empty();
}

void peer_t::dump_all(ostream& os) const {
    dump(os);
    client::dump_all(os);
}

