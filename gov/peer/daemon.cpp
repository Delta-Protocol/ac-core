/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#include "daemon.h"

#include <algorithm>

#include "protocol.h"

using namespace us::gov;
using namespace us::gov::peer;
using namespace std;

daemon::daemon(): m_edges(0) {
}

daemon::daemon(uint16_t port, uint16_t edges): m_edges(edges), auth::daemon(port,edges) {
}

daemon::~daemon() {
}

void daemon::daemon_timer() { // network mutation
    cout << "Connecting to new Neighbours" << endl;
    peers_t p=adjust_peer_number();
    check_latency(p);
}

void daemon::check_latency(const peers_t& a) {
    for (auto&i:a) {
        if (i->m_stage==peer_t::connected) {
            cout << "SENDING PING from check_latency" << endl;
            i->ping();
        }
    }
}

daemon::peers_t daemon::active() const {
    auto a=auth::daemon::active();
    return move( reinterpret_cast<peers_t&>(a) );
}

daemon::peers_t daemon::adjust_peer_number() {
    auto a=active();
    add_peers(a);
    purge_peers(a);
    return move(a);
}

void daemon::add_peers(peers_t& a) {
    int n=m_edges-a.asize()+1; //must be signed int
    unordered_set<string> exclude;
    for (auto i:a) if (i!=0) exclude.emplace(i->m_addr);
    int m=0;

    while(n>0) {
        string addr=get_random_peer(exclude);

        if (!addr.empty()) {
            if (exclude.find(addr)==exclude.end()) {
                exclude.emplace(addr);
                auto* p=create_client(0);

                string r=p->connect(addr,16672);
                if (likely(r.empty())) {
                    attach(p,false);
                    peer_t* pp=static_cast<peer_t*>(p);
                    a.push_back(pp);
                    ++m;
                }
                else {
                    delete p;
                }
            }
        }
        --n;
    }
    if (m>0) m_clients.read_sockets();
}

vector<peer_t*> daemon::in_service() const {
    return in_service(active());
}

vector<peer_t*> daemon::in_service(const peers_t& a) const {
    vector<peer_t*> ans;
    for (auto& i:a) {
        auto& c=static_cast<peer_t&>(*i);
        if (c.m_stage==peer_t::service) ans.push_back(&c);
    }
    return move(ans);
}

void daemon::purge_slow(peers_t&a) {
    peers_t copy;
    copy.reserve(a.size());
    for (auto& i:a) {
        if (i->m_stage==peer_t::exceed_latency) {
            i->disconnect();
        }
        else {
            copy.emplace_back(i);
        }
    }
    a=copy;
}

daemon::peers_t::iterator daemon::oldest(peers_t& v) const {
    peers_t::iterator o=v.end();
    auto cur=chrono::steady_clock::now();
    for (auto i=v.begin(); i!=v.end(); ++i) {
        if (*i==0) continue;
        auto& c=static_cast<peer_t&>(**i);
        if (c.m_since<cur) {
            cur=c.m_since;
            o=i;
        }
    }
    return o;
}

void daemon::purge_excess(peers_t& a) {
    while (a.asize()>m_edges) {
        auto i=oldest(a);
        if (i==a.end()) break;
        (*i)->disconnect();
        *i=0;
    }

    peers_t copy;
    copy.reserve(a.size());
    for (auto i:a) {
        if (i) copy.emplace_back(i);
    }

    a=copy;
}

void daemon::purge_peers(peers_t& a) {
    purge_slow(a);
    purge_excess(a);
}

void daemon::send(int num, peer_t* exclude, datagram* d) {
    auto a=active();
    if (num==0) { //all, no need to shuffle
        num=a.asize();
    }
    else {
        random_shuffle(a.begin(),a.end());
    }

    for(auto i=a.begin(); i!=a.end(); ++i) {
        if (*i==0) continue;
        if (*i==exclude) continue;
        (*i)->send(*d);
        (*i)=0;
        if (--num==0) break;
    }
    delete d;
}

void daemon::dump(ostream& os) const {
    os << "Hello from peer::daemon" << endl;
    os << "Max edges: " << m_edges << endl;
    os << "Thread pool size: " << m_pool->size() << endl;
    os << "Active: " << endl;
    active().dump(os);
}
