#include "auth_app.h"
#include "daemon.h"

using namespace std;
using namespace usgov;
using namespace usgov::blockchain::auth;

typedef usgov::blockchain::auth::app c;

constexpr array<const char*,policies_traits::num_params> policies_traits::paramstr;

c::app() {
	pool=new app_gut();
	policies_local=policies;
}

c::~app() {
	delete pool;
}

usgov::blockchain::peer_t::stage_t c::my_stage() const {
	if (cache_my_stage!=peer_t::unknown) return cache_my_stage;
	auto k=get_keys().pub.hash();
	if (db.nodes.find(k)!=db.nodes.end()) {
		cache_my_stage=peer_t::node;
	}
	else if (db.hall.find(k)!=db.hall.end()) {
		cache_my_stage=peer_t::hall;
	}
	else {
		cache_my_stage=peer_t::out;
	}
	return cache_my_stage;
}

void c::run() {
	while(!program::_this.terminated) {
		db.dump(cout);
		cout << "I am " << peer_t::stagestr[my_stage()] << endl;
		thread_::_this.sleep_for(chrono::seconds(10));
	}
}

void c::basic_auth_completed(peer_t* p) {
	cout << "APP auth_app, basic_auth_completed for " <<  p->pubkey <<  " " << p->pubkey.hash() << endl;
	if (p->pubkey==parent->peerd.id.pub) {  ///sysop connection, this connection requires a shell
		p->stage=peer_t::sysop;
		cout << "SYSOP " << endl;
		return;
	}
	{
	lock_guard<mutex> lock(db.mx_nodes);
	auto i=db.nodes.find(p->pubkey.hash());
	if (i!=db.nodes.end()) {
		p->stage=peer_t::node;
		cout << "NODE " << endl;
		return;
	}
	}
	{
	db_t::nodes_t::const_iterator i;
	{
	lock_guard<mutex> lock(db.mx_hall);
	i=db.hall.find(p->pubkey.hash());
	}
	if (i==db.hall.end()) {
		p->stage=peer_t::out;
		pool->to_hall.push_back(make_pair(p->pubkey.hash(),p->addr));
	}
	else {
		p->stage=peer_t::hall;
	}
	}
}

void c::add_policies() {
	lock_guard<mutex> lock(mx_pool);
	lock_guard<mutex> lock2(mx_policies);
	for (int i=0; i<policies_traits::num_params; ++i)
		(*pool)[i] = policies_local[i];
}

void c::add_growth_transactions(unsigned int seed) {

	double growth=policies[policies_traits::network_growth]; //percentage
	if (abs(growth)<1e-8) return; // no growth
	
	int min_growth=round(policies[policies_traits::network_min_growth]);
	default_random_engine generator(seed);
	db_t::nodes_t* src;
	db_t::nodes_t* dst;
	unordered_set<int> uniq;
	size_t maxr;
	int s;
	lock_guard<mutex> lock(db.mx_hall);
	lock_guard<mutex> lock2(db.mx_nodes);
	
	if (growth>=0) {
		size_t nh=db.hall.size();
		s=floor((double)nh*growth);
		src=&db.hall;
		dst=&db.nodes;
		if (s<min_growth) s=min_growth;
		if (s>nh) s=nh;
		maxr=nh-1;
		//cout << "grow the network with " << s << " nodes." << endl;
	}
	else {
		size_t nn=db.nodes.size();
		s=-floor((double)nn*growth);
		src=&db.nodes;
		dst=&db.hall;
		maxr=nn-1;
		//cout << "shrink the network with " << s << " nodes." << endl;
	}
	uniform_int_distribution<size_t> distribution(0,maxr);
	for (size_t i=0; i<s; ++i) { ///move s from hall to nodes
		auto p=src->begin();
		size_t r;
		while (true) {
			r=distribution(generator);
			if (!uniq.insert(r).second) continue;
			if (r>=src->size()) continue;
			break;				
		}
		advance(p,r);
		dst->emplace(*p);
		src->erase(p);
	}	
}

void c::on_begin_cycle() {
}

int usgov::blockchain::auth::app_gut::app_id() const {
	return app::id(); 
}

void usgov::blockchain::auth::app_gut::to_stream(ostream& os) const {
	os << to_hall.size() << " ";
	for (auto& i:to_hall) {
		os << i.first << " " << i.second << " ";
	}
	b::to_stream(os);
}

void usgov::blockchain::auth::app_gut::from_stream(istream& is) {
	int n;
	is >> n;
	to_hall.reserve(n);
	for (int i=0; i<n; ++i) {
		pubkey_t::hash_t pkeyh;
		address addr;
		is >> pkeyh;
		is >> addr;
		to_hall.push_back(make_pair(pkeyh,addr));
	}

	b::from_stream(is);
}


void usgov::blockchain::auth::app_gut2::to_stream(ostream& os) const {
	os << to_hall.size() << " ";
	for (auto& i:to_hall) {
		os << i.first << " " << i.second << " ";
	}
	b::b1::to_stream(os);
}

app_gut2* usgov::blockchain::auth::app_gut2::from_stream(istream& is) {
	app_gut2* g=new app_gut2();
	{
	int n;
	is >> n;
	for (int i=0; i<n; ++i) {
		pubkey_t::hash_t pkeyh;
		address addr;
		is >> pkeyh;
		is >> addr;
		g->to_hall.emplace(make_pair(pkeyh,addr));
	}
	}

	static_cast<b*>(g)->from_stream(is);
	return g;
}

void app::import(const blockchain::app_gut2& gg, const blockchain::pow_t&) {
	const app_gut2& g=static_cast<const app_gut2&>(gg);
	{
	lock_guard<mutex> lock(db.mx_hall);
	for (auto& i:g.to_hall) {
		{
		auto k=db.hall.find(i.first);
		if (k==db.hall.end()) db.hall.emplace(i);
		}
	}
	}
	{
	lock_guard<mutex> lock(mx_policies);
	for (int i=0; i<policies_traits::num_params; ++i) policies[i]=g[i];
	}

	auto seed=parent->get_seed();

	add_growth_transactions(seed);

	parent->update_peers_state();
	cache_my_stage=peer_t::unknown;
}

blockchain::peer_t::stage_t app::db_t::get_stage(const pubkeyh_t& key) const {
	{
	lock_guard<mutex> lock(mx_nodes);
	if (nodes.find(key)!=nodes.end()) return peer_t::node;
	}

	{
	lock_guard<mutex> lock(mx_hall);
	if (hall.find(key)!=hall.end()) return peer_t::hall;
	}
	return peer_t::out;
}



usgov::blockchain::app_gut* c::create_app_gut() {
	cout << "app: auth: create_app_gut " << endl;

	add_policies();
	lock_guard<mutex> lock(mx_pool);
	auto full=pool;
	pool=new app_gut();
	return full; //send collected transactions to the network
}

#include <random>

void c::db_t::dump(ostream& os) const {
	cout << "Auth app db dump" << endl;
	{
	lock_guard<mutex> lock(mx_nodes);
	cout << nodes.size() << " nodes:" << endl;
	for (auto& i:nodes) {
		cout << "  " << i.first << " " << i.second << endl;
	}
	}
	{
	lock_guard<mutex> lock(mx_hall);
	cout << hall.size() << " candidates in hall:" << endl;
	for (auto& i:hall) {
		cout << "  " << i.first << " " << i.second << endl;
	}
	}
}

string c::get_random_node(const pubkeyh_t& exclude, const unordered_set<string>& exclude_addrs) const {
	lock_guard<mutex> lock(db.mx_nodes);
	if (db.nodes.empty()) return "";
	uniform_int_distribution<> d(0, db.nodes.size()-1);
	for (int j=0; j<10; ++j) {
		auto i=db.nodes.begin();
		advance(i,d(parent->rng));
		if (i->first!=exclude && exclude_addrs.find(i->second)==exclude_addrs.end()) {
			return i->second;
		}
	}
	return "";
}

string c::shell_command(const string& cmdline) {
	ostringstream os;
	istringstream is(cmdline);
	string cmd;
	is >> cmd;
	if (cmd=="hello") {
		os << "Auth app shell. type h for help." << endl;
	}
	else if (cmd=="h" || cmd=="help") {
		os << "Auth app shell." << endl;
		os << "h|help              Shows this help." << endl;
		os << "p|policies [id vote]          ." << endl;
		os << "exit                Exits this app and returns to parent shell." << endl;
		os << "" << endl;
	}
	else if (cmd=="exit") {
	}
	else if (cmd=="p" || cmd=="policies") {
		int n=-1;
		double value;
		is >> n;
		is >> value;
		if (n>=0 && n<policies_traits::num_params) {
			lock_guard<mutex> lock(mx_policies);
			policies_local[n]=value;
		}
		else {
			os << "parameter " << n << " not found" << endl;
		}
		dump_policies(os);
	}
	else {
		os << "Unrecognized command" << endl;
	}
	return os.str();
}

void c::dump_policies(ostream& os) const {
	lock_guard<mutex> lock(mx_policies);
	os << policies_traits::num_params << " consensuated variables:" << endl;
	for (int i=0; i<policies_traits::num_params; ++i) {
		os << "  " << i << ". " << policies_traits::paramstr[i] << " [avg] consensuated value: " << policies[i] << " local value:" << policies_local[i] << endl;
	}
}

void c::dbhash(hasher_t& h) const {
	db.hash(h);
	lock_guard<mutex> lock(mx_policies);
	policies.hash(h);

}

void c::db_t::hash(hasher_t& h) const {
	{
	lock_guard<mutex> lock(mx_nodes);
	for (auto& i:nodes) {
		h << i.first << i.second;
	}
	}
	{
	lock_guard<mutex> lock(mx_hall);
	for (auto& i:hall) {
		h << i.first << i.second;
	}
	}
}

void c::db_t::clear() {
	{
	lock_guard<mutex> lock(mx_nodes);
    nodes.clear();
	}
	{
	lock_guard<mutex> lock(mx_hall);
    hall.clear();
	}
}


void c::clear() {
    {
	lock_guard<mutex> lock(mx_policies);
    policies.clear();
    }
    db.clear();

}





