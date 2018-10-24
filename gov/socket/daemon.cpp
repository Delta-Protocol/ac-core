/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#include "daemon.h"

#include <atomic>
#include <cassert>
#include <chrono>
#include <string>
#include <thread>
#include <utility>

#include "us/gov/signal_handler.h"
#include "us/gov/likely.h"
#include "ctpl_stl.h"
#include "datagram.h"
#include "peer_t.h"

using namespace us::gov::socket;
using namespace us::gov;

using namespace std::chrono_literals;

daemon::daemon():m_pool(0) {
}

daemon::daemon(uint16_t port, int edges)
: server(port), m_pool(new ctpl::thread_pool(edges)) {
}

daemon::~daemon() {
    delete m_pool;
}

void daemon::attach(client*c, bool wakeupselect) {
    assert(c!=0);
    assert(static_cast<peer_t*>(c)->get_parent()==0);
    static_cast<peer_t*>(c)->set_parent(this);
    server::attach(c, wakeupselect);
}

void daemon::on_finish() {
    server::on_finish();
}

void daemon::run() {
    if (m_pool==0) {
        cerr << "Error. No worker threads available" << endl;
        return;
    }

    thread listen(&server::run,this);
    thread_::_this.sleep_for(1s);
    {
        while(!thread_::_this.terminated) {
            daemon_timer();
            m_pool->dump(cout);
            thread_::_this.sleep_for(chrono::seconds(30));
        }
    }
    listen.join();
}

void daemon::dump(ostream& os) const {
    m_pool->dump(os);
}

void daemon::receive_and_process(client* c) {
    assert(m_pool!=0);
    auto* cl=static_cast<peer_t*>(c);

    if (likely(m_pool->n_idle()>0)) {
        m_pool->push([&,cl](int id) { process_work(cl); });
    }
    else {
        c->disconnect();
        ++m_perf.disconnections.thread_pool.full;
    }
}

void daemon::process_work(peer_t *c) {
    while(true) {
        auto r=c->recv();
        if (unlikely(!r.first.empty())) {
            assert(!r.second);
            break; //processed work
        }

        assert(r.second!=0);
        if (unlikely(!r.second->completed())) {
            delete r.second;
            break;
        }

        bool processed=process_work(c,r.second);
        if (!processed) {
            delete r.second;
            c->disconnect();
            ++m_perf.disconnections.datagram.unknown_service;
        }
        break;
    }

    c->store_busy(false);
    m_clients.read_sockets(); //might have more datagrams to process
}

bool daemon::process_work(socket::peer_t *c, datagram*d) {
    if (c->process_work(d)) {
        return true;
    }
    return false;
}

client* daemon::create_client(int sock) {
    return new peer_t(sock);
}

