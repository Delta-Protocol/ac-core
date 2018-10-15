#include "daemon.h"
#include "protocol.h"

using namespace us::gov;
using namespace us::gov::peer;
using namespace std;

daemon::daemon(): edges(0) {
}

daemon::daemon(uint16_t port, uint16_t edges): edges(edges), auth::daemon(port,edges) {
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
        if (i->stage==peer_t::connected) {
            cout << "SENDING PING from check_latency" << endl;
            i->ping();
        }
    }
}

daemon::peers_t daemon::connected_peers() const {
    auto a=active();
    peers_t r;
    for (auto& i : a) {
        if (i->stage==peer_t::connected) r.push_back(i);
    }
    return move(r);
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
    int n=edges-a.asize()+1; //must be signed int
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
    if (m>0) clients.read_sockets();
}

vector<peer_t*> daemon::in_service() const {
    return in_service(active());
}

vector<peer_t*> daemon::in_service(const peers_t& a) const {
    vector<peer_t*> ans;
    for (auto& i:a) {
        auto& c=static_cast<peer_t&>(*i);
        if (c.stage==peer_t::service) ans.push_back(&c);
    }
    return move(ans);
}

void daemon::purge_slow(peers_t&a) {
    peers_t copy;
    copy.reserve(a.size());
    for (auto& i:a) {
        if (i->stage==peer_t::exceed_latency) {
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
        if (c.since<cur) {
            cur=c.since;
            o=i;
        }
    }
    return o;
}

void daemon::purge_excess(peers_t& a) {
    while (a.asize()>edges) {
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
    os << "Max edges: " << edges << endl;
    os << "Thread pool size: " << m_pool->size() << endl;
    os << "Active: " << endl;
    active().dump(os);
}
