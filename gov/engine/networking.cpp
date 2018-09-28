#include "networking.h"
#include "daemon.h"

using namespace us::gov::engine;
using namespace us::gov;
using namespace std;

typedef us::gov::engine::networking c;

c::networking(engine::daemon* parent, const string& home): b(home), parent(parent) {
}

c::networking(uint16_t port, uint16_t edges, engine::daemon* parent, const vector<string>& seed_nodes, const string& home): b(port, edges, home), parent(parent), seed_nodes(seed_nodes) {

}

const c::keys& c::get_keys() const {
    return parent->id;
}

vector<us::gov::relay::peer_t*> c::get_nodes() {
                auto a=parent->get_nodes();
                return *reinterpret_cast<vector<relay::peer_t*>*>(&a);
}

us::gov::socket::client* c::create_client(int sock) {
                return new peer_t(sock);
                //p->parent=this;
                //return p;
}

void c::dump(ostream& os) const {
                os << "Active edges:" << endl;
                auto a=active();
//              vector<peer_t*>& v=reinterpret_cast<vector<peer_t*>&>(a);
                for (auto& i:a) {
                    static_cast<peer_t*>(i)->dump_all(os);
                }
}


string c::get_random_peer(const unordered_set<string>& exclude_addrs) const { //returns ipaddress
//cout << "exclude: ";
//for (auto&i:exclude_addrs) cout << i << " ";
//cout << endl;

//cout << "my pubkey is " << id.pub << endl;
	auto n=parent->get_random_node(exclude_addrs);
//cout << "auth_app->get_random_node=" << n << endl;
	if (unlikely(n.empty()) && !seed_nodes.empty()) {
//cout << "no nodes . using seeds " << endl;
		uniform_int_distribution<> d(0, seed_nodes.size()-1);
		for (int j=0; j<10; ++j) {
			auto i=seed_nodes.begin();
			advance(i,d(parent->rng));
			if (exclude_addrs.find(*i)==exclude_addrs.end()) {
//cout << "found " << *i << endl;
				return *i;
			}
		}
	}
//	if (!n.empty()) cout << "Random node at " << n << endl;
	return move(n);
}

dfs::peer_t* c::get_random_edge(const dfs::peer_t* exclude) const {
    return parent->get_random_edge(static_cast<const peer_t*>(exclude));
}

dfs::peer_t* c::get_random_edge() const {
    return parent->get_random_edge();
}

bool c::process_evidence(datagram*d) {
	return parent->process_evidence(d);
}

bool c::process_work(socket::peer_t *c, datagram*d) { //TOPMOST DGRAM ROUTER
	if (protocol::is_node_protocol(d->service)) { //high priority
		if (parent->process_work(static_cast<peer_t*>(c),d)) return true;
	}

	if (protocol::is_app_query(d->service)) {
		if (parent->process_app_query(static_cast<peer_t*>(c),d)) return true;
	}

	if (b::process_work(c,d)) {
		return true;
	}

#ifdef DEBUG
cout << "WARNING, NOT handled at all " << d->service << endl;
#endif

	return false;
}

bool c::process_work_sysop(peer::peer_t *c, datagram*d) {
	if (b::process_work(c,d)) return true;
	return false;
}
