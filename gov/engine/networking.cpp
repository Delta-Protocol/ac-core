#include "networking.h"
#include "daemon.h"

using namespace us::gov::engine;
using namespace us::gov;
using namespace std;

typedef us::gov::engine::networking c;

networking::networking(engine::daemon* parent, 
                       const string& home): b(home), m_parent(parent) {
}

networking::networking(uint16_t port, uint16_t edges, 
                       engine::daemon* parent, const vector<string>& seed_nodes, 
                       const string& home): b(port, edges, home), 
                                            m_parent(parent), 
                                            m_seed_nodes(seed_nodes) {
}

const networking::keys& c::get_keys() const {
    return m_parent->get_id();
}

vector<us::gov::relay::peer_t*> 
networking::get_nodes() {
    auto a=m_parent->get_nodes();
    return *reinterpret_cast<vector<relay::peer_t*>*>(&a);
}

us::gov::socket::client* 
networking::create_client(int sock) {
    return new peer_t(sock);
}

void networking::dump(ostream& os) const {
    os << "Active edges:" << endl;
    auto a=active();
    for (auto& i:a) {
        static_cast<peer_t*>(i)->dump_all(os);
    }
}


string networking::get_random_peer(const unordered_set<string>& exclude_addrs) const {
    auto n=m_parent->get_random_node(exclude_addrs);
    if (unlikely(n.empty()) && !m_seed_nodes.empty()) {
        uniform_int_distribution<> d(0, m_seed_nodes.size()-1);
        for (int j=0; j<10; ++j) {
            auto i=m_seed_nodes.begin();
            advance(i,d(m_parent->get_rng()));
            if (exclude_addrs.find(*i)==exclude_addrs.end()) {
                return *i;
            }
        }
    }
    return move(n);
}

dfs::peer_t* networking::get_random_edge(const dfs::peer_t* exclude) const {
    return m_parent->get_random_edge(static_cast<const peer_t*>(exclude));
}

dfs::peer_t* networking::get_random_edge() const {
    return m_parent->get_random_edge();
}

bool networking::process_evidence(datagram*d) {
    return m_parent->process_evidence(d);
}

bool networking::process_work(socket::peer_t *c, datagram*d) { 
    if (protocol::is_node_protocol(d->service)) { 
        if (m_parent->process_work(static_cast<peer_t*>(c),d)) 
            return true;
    }
    if (protocol::is_app_query(d->service)) {
        if (m_parent->process_app_query(static_cast<peer_t*>(c),d)) 
            return true;
    }
    if (b::process_work(c,d)) {
        return true;
    }
    return false;
}

bool networking::process_work_sysop(peer::peer_t *c, datagram*d) {
    if (b::process_work(c,d)) 
        return true;
    return false;
}
