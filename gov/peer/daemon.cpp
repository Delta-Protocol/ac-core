#include "daemon.h"
#include "protocol.h"

typedef us::gov::peer::daemon c;
using namespace us::gov;
using namespace us::gov::peer;
using namespace std;

c::daemon(): edges(0) {
}

c::daemon(uint16_t port, uint16_t edges): edges(edges), b(port,edges) {
}

c::~daemon() {
}

socket::client* c::create_client(int sock) {
	auto p=new peer_t(sock);
	p->parent=this;
	return p;
}

void c::daemon_timer() { // network mutation
	pub_t p=adjust_peer_number();

	check_latency(p);

//	auto nis=in_service();
//	report_in_service(nis);
}
/*
void c::report_in_service(vector<peer_t*>&) {
}
*/
void c::check_latency(const pub_t& a) {
	for (auto&i:a) {
		if (i->stage==peer_t::connected) {
			cout << "SENDING PING from check_latency" << endl;
			i->ping();
		}
	}		
}

c::pub_t c::connected_peers() const { 
	auto a=active();
	pub_t r;
	for (auto&i:a) {
		if (i->stage==peer_t::connected) r.push_back(i);
	}
	return move(r); 
}

c::pub_t c::active() const { 
	auto a=b::active();
	return move( reinterpret_cast<pub_t&>(a) ); 
}

c::pub_t c::adjust_peer_number() {
	auto a=active();
	add_peers(a);
	purge_peers(a);
	return move(a);
}

bool c::process_evidence(peer_t *c, datagram*d) {
	return false;
}

void c::clear_evidences() {
	evidences.clear();
}

bool c::process_work(socket::peer_t *c, datagram*d) {
	if (protocol::is_evidence(d->service)) {
		datagram::hash_t h=d->compute_hash();
		{
		lock_guard<mutex> lock(mx_evidences);
		if (evidences.find(h)!=evidences.end()) {
		delete d;
		return true;
		}
		evidences.emplace(h);
		}
		return process_evidence(static_cast<peer_t*>(c),d);
	}

	bool ispong=d->service==protocol::pong;
	if (c->process_work(d)) {
		if (ispong) {
			{
			unique_lock<mutex> lock(mx);
			ready=true;
			}
			cv.notify_all(); //wake up to start with auth
		}
		return true;
	}
	return false;
}

void c::add_peers(pub_t& a) {
cout << "add peers" << endl;
	auto n=edges-a.asize()+1;
	unordered_set<string> exclude;
	for (auto i:a) if (i!=0) exclude.emplace(i->addr);
	while(n>0) {
		string addr=get_random_peer(exclude);
		if (!addr.empty()) {
cout << "addr " << addr << endl;
			if (exclude.find(addr)==exclude.end()) {
				exclude.emplace(addr);
				auto* p=create_client(0);
cout << "connecting to address " << addr << endl;
				if (p->connect(addr,16672)) {
cout << "XXXXXXXXXXXXXX connected to address " << addr << endl;
					peer_t* pp=static_cast<peer_t*>(p);
					a.push_back(pp);
				}
				else {
cout << "XXXXXXXXXXXXXX " << addr << endl;
					cout << "peerd: unable to connect, deleting peer" << endl;
					delete p;
				}
			}
		}
		--n;
	}
}

vector<peer_t*> c::in_service() const {
	return in_service(active());
}

vector<peer_t*> c::in_service(const pub_t& a) const {
	vector<peer_t*> ans;
	for (auto& i:a) {
		auto& c=static_cast<peer_t&>(*i);
		if (c.stage==peer_t::service) ans.push_back(&c);
	}
	return move(ans);
}

void c::purge_slow(pub_t&a) {
	pub_t copy;
	copy.reserve(a.size());
	for (auto& i:a) {
		if (i->stage==peer_t::exceed_latency) {
			cout << "peer: daemon: disconnection slow peer" << endl;
			disconnect(i);
		}
		else {
			copy.emplace_back(i);
		}
	}
	a=copy;
}

c::pub_t::iterator c::oldest(pub_t& v) const {
	pub_t::iterator o=v.end();
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

void c::set_mode(int mode) { //0 tor; 1 ip4
	for (auto i:active()) i->set_mode(mode);
}

void c::purge_excess(pub_t& a) {
	while (a.asize()>edges) {
		auto i=oldest(a);
		if (i==a.end()) break;
		disconnect(*i);
		*i=0;
	}
	pub_t copy;
	copy.reserve(a.size());
	for (auto i:a) {
		if (i) copy.emplace_back(i);
	}
	a=copy;
}

void c::purge_peers(pub_t& a) {
	purge_slow(a);
	purge_excess(a);
}

void c::send(int num, peer_t* exclude, datagram* d) {
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

void c::dump(ostream& os) const {
	os << "Hello from peer::daemon" << endl;
	os << "Max edges: " << edges << endl;
	os << "Thread pool size: " << pool->size() << endl;
	os << "Active: " << endl;
	active().dump(os);

}

