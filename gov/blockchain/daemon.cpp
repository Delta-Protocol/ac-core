#include "daemon.h"
#include <us/gov/auth.h>
#include "protocol.h"
#include <us/gov/signal_handler.h>

using namespace us::gov::blockchain;
using namespace std;

typedef us::gov::blockchain::daemon c;

void c::constructor() {
	auth_app=new auth::app(peerd.id.pub);
	add(auth_app);
}

c::daemon(const keys& k): rng(chrono::system_clock::now().time_since_epoch().count()), peerd(k, this), sysops(*this), syncdemon(this) { //dependency order
	constructor();	
}

c::daemon(const keys& k, const string& home, uint16_t port, uint8_t edges, const vector<string>& seed_nodes): rng(chrono::system_clock::now().time_since_epoch().count()), peerd(k, port, edges, this, seed_nodes), home(home), syncdemon(this), sysops(*this) { //dependency order
	constructor();	
}

c::~daemon() {
	delete pool;
}

void c::add(app*app) {
	if (apps_.find(app->get_id())!=apps_.end()) {
		cerr << "Fatal error: App collision. Two apps with same id." << endl;
		exit(1);
	}
	//app->parent=this;
	apps_.emplace(app->get_id(),app);
}

void c::start_new_blockchain(const string& addr) {
    assert(!addr.empty());
    auto pool=new diff();
    auth_app->pool->to_hall.push_back(make_pair(peerd.id.pub.hash(),addr));
    auto* mg=create_local_deltas();
    assert(mg!=0);
    pool->allow(*mg);
    pool->add(mg);
    pool->end_adding();
    save(*pool);
    if (!import(*pool)) {
        cerr << "Error creating blockchain" << endl;
        exit(1);
    }
    delete pool;
}


c::syncd::syncd(daemon* d): d(d), head(0),cur(0),tail(0) {
}

void c::syncd::run() {
	while(!program::_this.terminated) {
		cout << "SYNCD: Start" << endl;
		hash_t he(0), ta(0), cu(0);
		{
			vector<hash_t> patches;
			while(true) {
				cout << "SYNCD: begin iteration " << endl;
				{
				lock_guard<mutex> lock(mx); 
				if (cur==tail) break;
				if (head.is_zero()) break;
				if (cur.is_zero()) break;
				he=head;
				ta=tail;
				cu=cur;
				}
				if (cu==he) patches.clear();
				cout << "SYNCD: head:" << he << " cur: " << cu << " tail: " << ta << " ;" << patches.size() << " patches" << endl;
				hash_t prev;
				if (d->get_prev(cu,prev)) { //file exists, read it 
					cout << "SYNCD: found file for " << cu << endl;
					patches.push_back(cu);
					lock_guard<mutex> lock(mx); 
					cur=prev;
				}
				else {
					cout << "SYNCD: querying file for " << cu << endl;
					d->query_block(cu);
					cout << "SYNCD: going to sleep for 5 secs." << endl;
					wait(chrono::seconds(5)); //TODO better
					cout << "SYNCD: waked up " << endl;
				}
				if (program::_this.terminated) return;
			}
			if (!patches.empty()) {
				if (d->patch_db(patches)) {
					lock_guard<mutex> lock(mx); 
					tail=*patches.begin();
				}
                else {
                    d->clear();
                }
			}
		}
		cout << "SYNCD: going to sleep indefinitely" << endl;
		wait();
		cout << "SYNCD: waked up " << endl;
	}
}

void c::syncd::update(const diff::hash_t& h, const diff::hash_t& t) {
	{
	lock_guard<mutex> lock(mx); 
	cur=head=h;
	tail=t;
	resume=true;
	}
	update();
}
void c::syncd::update(const diff::hash_t& t) {
	{
	lock_guard<mutex> lock(mx); 
	tail=t;
	cur=head;
	resume=true;
	}
	update();
}
void c::syncd::update() {
	cout << "SYNCD: received wakeup signal " << endl;
	cv.notify_all();		
}
void c::syncd::wait() {
	unique_lock<mutex> lock(mx);
	cv.wait(lock, [&]{ return resume || program::_this.terminated; });
	resume=false;
}
void c::syncd::wait(const chrono::steady_clock::duration& d) {
	unique_lock<mutex> lock(mx);
	cv.wait_for(lock, d, [&]{ return resume || program::_this.terminated; });
	resume=false;
}
void c::syncd::on_finish() {
	cv.notify_all();		
}

bool c::syncd::in_sync() const {
	lock_guard<mutex> lock(mx); 
	return head==tail;
}

void c::syncd::dump(ostream& os) const {
	os << "Greetings from syncd" << endl;
	os << "in sync: " << boolalpha << in_sync() << endl;
	lock_guard<mutex> lock(mx); 
	os << "head: " << head << endl;
	os << "cur: " << cur << endl;
	os << "tail: " << tail << endl;
}


#include <fstream>
bool c::patch_db(const vector<diff::hash_t>& patches) { //this is syncd thread
	cout << "Applying " << patches.size() << " patches" << endl;
	for (auto i=patches.rbegin(); i!=patches.rend(); ++i) {
		const diff::hash_t& hash=*i;
		string filename=blocksdir()+"/"+hash.to_b58();
		ifstream is(filename);
		if (!is.good()) {
			cerr << "corrupt filename 1 " << filename << endl;
			remove(filename.c_str());
			return false;
		}
		diff* b=diff::from_stream(is);
		if (!b) {
			cerr << "corrupt filename 2 " << filename << endl;
			remove(filename.c_str());
			return false;
		}
		if (b->hash()!=hash) {
			cerr << "corrupt filename 3 " << filename << endl;
			remove(filename.c_str());
			delete b;
			return false;
		}
		if (!import(*b)) {
			delete b;
			return false;
		}
		delete b;
	}
	
	diff::hash_t lbi;
	{
	lock_guard<mutex> lock(mx_import); 
	lbi=last_block_imported;
	}
cout << "Updating tail of syncd: " << lbi << endl;
	syncdemon.update(lbi);	
	return true;
}


const diff::hash_t& c::dbhash_off() const {
	if (!cached_dbhash_ok) {
		diff::hasher_t h;
		for (auto&i:apps_) {
			i.second->dbhash(h);
		}
		h.finalize(cached_dbhash);	
		cached_dbhash_ok=true;
	}
	return cached_dbhash;
}


diff::hash_t c::get_last_block_imported() const {
	lock_guard<mutex> lock(mx_import); 
	return last_block_imported;
}


void c::stage1(cycle_data& data) {
cout << "stage1 NB1 " << data.new_block << endl;
	on_begin_cycle();
	if (!data.new_block) return;
	if (auth_app->my_stage()!=peer_t::node) return;
cout << "stage1 VOTETIP " << endl;
	vote_tip(*data.new_block);
cout << "NB1 " << data.new_block << endl;
cout << "/stage1" << endl;
}

bool c::stage2(cycle_data& data) {
cout << "stage2 votes.size=" << votes.size() << endl;
	diff::hash_t hash=votes.select();		
	cout << "NB Voting process result: diff with hash " << hash << endl;
	cout << "Last block imported (syncd tail) before importing=" << get_last_block_imported() << endl;
	if (!hash.is_zero()) {
		cout << "NB2 " << data.new_block << endl;

		if (data.new_block) {
			cout << "data.new_block->hash() " << data.new_block->hash() << endl;
			if (hash==data.new_block->hash()) {
		//cout << "save" << endl;
				save(*data.new_block);
				if (!import(*data.new_block)) {
                    clear();
					//assert(false);
					//exit(1);
				}
			}
			delete data.new_block;
			data.new_block=0;
		}
		cout << "Last block imported before updating syncd (syncd tail)=" << get_last_block_imported() << endl;
		syncdemon.update(hash,get_last_block_imported()); //head,tail
		while (!syncdemon.in_sync() && !program::_this.terminated) {
			cout << "syncing" << endl;
			thread_::_this.sleep_for(chrono::seconds(1));
			if (data.cycle.get_stage()!=cycle_t::local_deltas_io) break;
		}
		if (data.cycle.get_stage()!=cycle_t::local_deltas_io) return false;
	}
//	if (auth_app->my_stage()!=peer_t::node) return false;

	//int min_in_week=0;
	//b->checkpoint=min_in_week==0; //once a week the closure block contains a ref to a checkpoint block
    if (!auth_app->is_node()) return false;

	auto* mg=create_local_deltas(); //send my gut to the network
	if (mg!=0) {
		send(*mg);
		{
		lock_guard<mutex> lock(mx_pool);
		pool->allow(*mg);
		pool->add(mg);
		}
	}
	return true;
}

void c::stage3(cycle_data& data) {
cout << "stage3 NB3 " << data.new_block << endl;
	assert(data.new_block==0);
	{
	lock_guard<mutex> lock(mx_pool);
	pool->end_adding();
	data.new_block=pool;
	pool=new diff();
	}
cout << "NB3 2 " << data.new_block << endl;
	data.new_block->prev=get_last_block_imported();
}

void c::load_head() {
        string filename=blocksdir()+"/head";
        ifstream is(filename);
        diff::hash_t head(0);
        if (is.good()) is >> head;
        syncdemon.update(head,get_last_block_imported());
}

void c::run() {
	//if (miners_size()>1000) peerd.set_ip4(); else peerd.set_tor();

	thread peers(&networking::run,&peerd);

	vector<thread> apps_threads;
	apps_threads.reserve(apps_.size());
	for (auto& i:apps_) {
		auto p=dynamic_cast<runnable_app*>(i.second);
		if (p!=0) apps_threads.emplace_back(thread(&runnable_app::run,p));
	}

	signal_handler::_this.add(&syncdemon);

	thread synct(&syncd::run,&syncdemon);

	cycle_data data;
	thread_::_this.sleep_for(chrono::seconds(5));

	assert(pool==0);
	pool=new diff();

	load_head();

	while(!program::_this.terminated) { // main loop
		data.cycle.wait_for_stage(cycle_t::new_cycle);
		stage1(data);
		data.cycle.wait_for_stage(cycle_t::local_deltas_io);
		if (!stage2(data)) continue;
		data.cycle.wait_for_stage(cycle_t::consensus_vote_tip_io);
		stage3(data);
	}

	synct.join();
	for (auto& i:apps_threads) i.join();
	peers.join();
}

string c::networking::get_random_peer(const unordered_set<string>& exclude_addrs) const { //returns ipaddress
cout << "exclude: ";
for (auto&i:exclude_addrs) cout << i << " ";
cout << "my pubkey is " << id.pub << endl;
	auto n=parent->get_random_node(exclude_addrs);
cout << "auth_app->get_random_node=" << n << endl;
	if (n.empty() && !seed_nodes.empty()) {
cout << "no nodes . using seeds " << endl;
		uniform_int_distribution<> d(0, seed_nodes.size()-1);
		for (int j=0; j<10; ++j) {
			auto i=seed_nodes.begin();
			advance(i,d(parent->rng));
			if (exclude_addrs.find(*i)==exclude_addrs.end()) {
cout << "found " << *i << endl;
				return *i;
			}
		}
		
	}
	if (!n.empty()) cout << "Random node at " << n << endl;
	return move(n);
}

#include <us/gov/crypto/base58.h>
#include <fstream>

string c::blocksdir() const {
    return home+"/blocks";
}

void c::save(const diff& bl) const {
	{
cout << "file " << blocksdir()+"/"+bl.hash().to_b58() << endl;
	ofstream os(blocksdir()+"/"+bl.hash().to_b58());
	bl.to_stream(os);
	}
	ifstream is(blocksdir()+"/"+bl.hash().to_b58());  //TODO remove this check
	diff*b=diff::from_stream(is);
	if (!b) {
		cout << "ERROR A" << endl;
		exit(1);
	}
	if (b->hash()!=bl.hash()) {
		cout << b->hash() << " " << bl.hash() << endl;
		{
		ofstream os(blocksdir()+"/"+bl.hash().to_b58()+"_");
		b->to_stream(os);
		}
		cout << "ERROR B " << (blocksdir()+"/"+bl.hash().to_b58()+"_") << endl;
		exit(1);
	}
	delete b;	
}
/*
void c::save_block(const string& content) const {
	unsigned char h[crypto::double_hash256::OUTPUT_SIZE];
	crypto::double_hash256 h;
	h.write(content);
	h.finalize(hash);
	string hash=crypto::b58::encode(h,h+crypto::double_hash256::OUTPUT_SIZE);
	ofstream os(blocksdir+"/"+hash);
	bl.to_stream(os);
}
*/

string c::get_random_node(const unordered_set<string>& exclude_addrs) const {
	return auth_app->get_random_node(rng,exclude_addrs);
}

void c::query_block(const diff::hash_t& hash) {
	auto n=get_random_edge();
	if (n==0) return;
//	cout << "querying diff " << hash << " to " << n->pubkey << endl;
	n->send(protocol::query_block,hash.to_b58());
}

string c::load_block(const string& block_hash_b58) const {
	ifstream is(blocksdir()+"/"+block_hash_b58);
	if (!is.good()) return "";
	return string((istreambuf_iterator<char>(is)), (istreambuf_iterator<char>()));
}


string c::load_block(const diff::hash_t& hash) const {
	ifstream is(blocksdir()+"/"+hash.to_b58());
	if (!is.good()) return "";
	return string((istreambuf_iterator<char>(is)), (istreambuf_iterator<char>()));
}


#include <sys/stat.h>

bool c::file_exists(const string& f) {
	struct stat s;
	stat(f.c_str(), &s);
	return S_ISREG(s.st_mode);
}


bool c::get_prev(const diff::hash_t& h, diff::hash_t& prev) const {
	string filename=blocksdir()+"/"+h.to_b58();
	if (!file_exists(filename)) return false;
	ifstream is(filename);
	if (!is.good()) return false;
	is >> prev;
	//if (prev.size()==1) prev.clear(); //genesis block
	return true;
}

peer_t* c::get_random_edge() {
//	vector<peer_t*> n=get_nodes();
    auto n=get_people(); //peerd.in_service();   sysops are not taken into account
    cout << n.size() << " nodes available" << endl;
	if (n.empty()) return 0;
	uniform_int_distribution<> d(0, n.size()-1);
	auto i=n.begin();
	advance(i,d(rng));
    return reinterpret_cast<peer_t*>(*i);
}

vector<peer_t*> c::get_nodes() {
	vector<peer_t*> v;
	for (auto& i:peerd.in_service()) {
		auto p=reinterpret_cast<peer_t*>(i);
		if (p->stage==peer_t::node) v.push_back(p);
	}
	return v;
}

vector<peer_t*> c::get_people() {
	vector<peer_t*> v;
	for (auto& i:peerd.in_service()) {
		auto p=reinterpret_cast<peer_t*>(i);
		if (p->stage==peer_t::out || p->stage==peer_t::hall || p->stage==peer_t::node) v.push_back(p);
	}
	return v;
}

void c::update_peers_state() {
	for (auto& i:peerd.in_service()) {
		auto p=reinterpret_cast<peer_t*>(i);
		if (p->stage!=peer_t::sysop)
			p->stage=auth_app->db.get_stage(p->pubkey.hash());
	}
}


void c::send(const local_deltas& g, peer_t* exclude) {
	ostringstream os;
	g.to_stream(os);
	string msg=os.str();
	for (auto& i:get_nodes()) {
		if (i==exclude) continue; //dont relay to the original sender
		i->send(protocol::local_deltas,msg);
	}
}

void c::send(const datagram& g, peer_t* exclude) {
	for (auto& i:get_nodes()) {
		if (i==exclude) continue; //dont relay to the original sender
		i->send(g);
	}
}

local_deltas* c::create_local_deltas() {
	cout << "blockchain: create_local_deltas" << endl;
	auto* mg=new local_deltas();

	{
    lock_guard<mutex> lock(peerd.mx_evidences); //pause reception of evidences while changing app pools
	for (auto&i:apps_) {
  cout << "===>create app gut for app " << i.first << endl;
		auto* amg=i.second->create_local_delta(); //
		if (amg!=0) {
			mg->emplace(i.first,amg);
		}
	}
	peerd.clear_evidences(); //evidences are recorded to decide if relay them or not, it is emptied on each cycle, it is safe to forget about them as they are tied to a parent block, for so they cannot be succesfully executed after the blockain grows 1 block
	}

	if (mg->empty()) {
		delete mg;
		return 0;
	}
	mg->sign(peerd.id);
	return mg;
}

void c::vote_tip(const diff& b) {
	const diff::hash_t& h=b.hash();
	//assert(!h.empty());
	votes.add(peerd.id.pub.hash(),h);

cout << "voting for tip be: " << h << endl;
	string signature=crypto::ec::instance.sign_encode(peerd.id.priv,h.to_b58());
	ostringstream os;
	os << h << " " << peerd.id.pub << " " << signature;
	string msg=os.str();
	for (auto& i:get_people()) {
		i->send(protocol::vote_tip,msg);
	}
}

void c::process_vote_tip(peer_t *c, datagram*d) {
	auto s=d->parse_string();
	delete d;
	istringstream is(s);
	string block_hash_b58;
	is >> block_hash_b58;
	pubkey_t pubkey;
	is >> pubkey;
	string signature;
	is >> signature;
cout << "Received vote from " << pubkey << " head be " << block_hash_b58 << endl;
	if (!crypto::ec::instance.verify(pubkey, block_hash_b58, signature)) {
		cout << "WRONG SIGNATURE in vote." << endl;
		//BAN this peer, sent a vote with wrong signature
		return;
	}
	if (votes.add(pubkey.hash(),diff::hash_t::from_b58(block_hash_b58))) {
		if (auth_app->my_stage()==peer_t::node) { //dont relay if I am not a node
			for (auto& i:get_people()) {
				if (i==c) continue; //dont relay to the original sender
				i->send(protocol::vote_tip,s);
			}
		}
	}

}
#include <us/gov/likely.h>
void c::process_incoming_local_deltas(peer_t *c, datagram*d) {
	auto s=d->parse_string();
	delete d;
//cout << "RECeived miner gut" << endl;
//cout << "----" << endl;
//cout << s << endl;
//cout << "----" << endl;
	istringstream is(s);
	local_deltas* g=local_deltas::from_stream(is);
	if (!g) return;
//cout << "----" << endl;
//g->to_stream(cout);
//cout << "----" << endl;
	/// discard if not properly signed
	if (unlikely(!g->verify())) {
		delete g;
		cout << "Failed verification of incoming local_deltas" << endl;
		return;
	}

	bool allowed;
	{
	lock_guard<mutex> lock(mx_pool);
	allowed=pool->allow(*g);
	}
	if (!allowed) { /// If it is not the first time we receive a gut from this node discard it, and don't relay
		delete g;
		return;
	}
	send(*g,c); //relay asap, dont relay to the original sender

	/// Add to pool
	{
	lock_guard<mutex> lock(mx_pool);
	pool->add(g);
	}

}

void us::gov::blockchain::cycle_t::wait_for_stage(stage ts) {
	using namespace chrono;
		//this_thread::sleep_until();
	time_point now=system_clock::now();
	duration tp = now.time_since_epoch();
	minutes m = duration_cast<minutes>(tp);
	tp-=m; 
	seconds s = duration_cast<seconds>(tp); //seconds in this minute
//cout << dec << s.count() << " <? " << ts << endl;
	int n=ts; //stage boundaries in seconds within this minute
	if (s.count()>n) n+=60;
cout << "Cycle: current second is " << s.count() << ". I'll sleep for " << (n-s.count()) << endl;
	thread_::_this.sleep_for(seconds(n-s.count()));
	cout << "blockchain: daemon: Starting stage: " << str(ts) << endl;
}

string us::gov::blockchain::cycle_t::str(stage s) const {
	switch(s) {
		case new_cycle: return "new_cycle"; break;
		case local_deltas_io: return "local_deltas_io"; break;
		case consensus_vote_tip_io: return "consensus_vote_tip_io"; break;
	}
	return "?";
}
us::gov::blockchain::cycle_t::stage us::gov::blockchain::cycle_t::get_stage() {
	using namespace chrono;
	time_point now=system_clock::now();
	duration tp = now.time_since_epoch();
	minutes m = duration_cast<minutes>(tp);
	tp-=m;
	seconds s = duration_cast<seconds>(tp);
	int sec=s.count();
	if (sec<local_deltas_io) return new_cycle;
	if (sec<consensus_vote_tip_io) return local_deltas_io;
	return consensus_vote_tip_io;
}

void c::on_begin_cycle() {
	cout << "blockchain daemon: on_begin_cycle" << endl;
/*
	for (auto&i:apps_) {
		i.second->on_begin_cycle();
	}
*/
}

void c::process_query_block(peer_t *c, datagram*d) {
	auto hash_b58=d->parse_string();
//cout << "Received a petition of block " << hash_b58 << " from " << c->pubkey << endl;
	delete d;
	string content=load_block(hash_b58);
	if (content.empty()) {
		cout << "block not found in HD, ignoring." << endl;
		return;
	}
/*
	cout << "Sending content of block " << hash_b58 << endl;
	cout << "---BEGIN--- " << content.size() << " bytes" << endl;
	cout << content << "<" << endl;
	cout << "---END---" << endl;
*/
	c->send(protocol::block,content);
}

void c::process_block(peer_t *c, datagram*d) {
	auto content=d->parse_string();
	delete d;
	istringstream is(content);
/*
	cout << "Received content of block " << endl;
	cout << "---BEGIN--- " << content.size() << " bytes" << endl;
	cout << content << "<" << endl;
	cout << "---END---" << endl;
*/
	diff* b=diff::from_stream(is);
	if (b==0) return;
	cout << "Received content of block " << b->hash() << " from " << c->addr << endl;


	string filename=blocksdir()+"/"+b->hash().to_b58();
	if (!file_exists(filename)) { //TODO write under different name and then rename atomically
		cout << "Saving it to disk " << filename << endl;
		{
		ofstream os(filename);
		os << content;
		}
		cout << "waking up sync demon after saving the block" << endl;
		syncdemon.update();
	}
	else {
		cout << "File exists, ignoring. " << filename << endl;
	}
	delete b;
}

bool c::networking::process_evidence(peer::peer_t *c, datagram*d) {
//cout << "PROCESSING EVIDENCE " << d->service << endl;
	return parent->process_evidence(reinterpret_cast<peer_t*>(c),d);
}

bool c::networking::process_work(socket::peer_t *c, datagram*d) {
cout << "PROCESSING DATAGRAM " << d->service << endl;
	if (protocol::is_node_protocol(d->service)) { //high priority
cout << "NODE PROTOCOL " << d->service << endl;
		if (parent->process_work(static_cast<peer_t*>(c),d)) return true;
cout << "NOT handled by specialists! " << d->service << endl;
	}
	if (protocol::is_app_query(d->service)) {
cout << "QUERY PROTOCOL " << d->service << endl;
		if (parent->process_app_query(static_cast<peer_t*>(c),d)) return true;
cout << "NOT handled by specialists! " << d->service << endl;
	}


	if (b::process_work(c,d)) { //ping, evidences, auth
		return true;
	}
cout << "WARNING, NOT handled at all " << d->service << endl;
	return false;
}
bool c::networking::process_work_sysop(peer::peer_t *c, datagram*d) {
//cout << "BLOCKCHAIN: A" << endl;
	if (b::process_work(c,d)) return true;
//cout << "BLOCKCHAIN: B" << endl;
	return false;
}


bool c::process_evidence(peer_t *c, datagram*d) {
        
	send(*d, c); //relay


	bool processed=false;
	for (auto&i:apps_) {
		if (i.second->process_evidence(c,d)) {
			processed=true;
			//break; //any app can use evidences designed by other apps
		}
	}
	return processed;
}

bool c::process_app_query(peer_t *c, datagram*d) {
cout << "BLOCKCHAIN: process_query " << d->service << endl;
	bool processed=false;
	for (auto&i:apps_) {
//		if (!i.second->in_service()) continue;
		if (i.second->process_query(c,d)) {
			processed=true;
			break;
		}
	}
//cout << "d" << endl;

	return processed;
}

bool c::process_work(peer_t *c, datagram*d) {
cout << "BLOCKCHAIN: PW process_work " << d->service << endl;

	if (d->service==protocol::sysop) {
cout << "PW sysop" << endl;
		bool alowed_sysop=sysop_allowed;
		if (c->stage==peer_t::sysop) { //Is peer a sysop?
			if (!sysop_allowed) {
				delete d;
				c->send(new datagram(protocol::sysop,"Sysop operation is not allowed."));
				c->disconnect();
				return true;
			}
			return sysops.process_work(c, d); //traslate the msg to sysopland
		}
		else {
cout << "Disconnecting, peer is not a sysop " << c->stage << endl;
			delete d;
			c->disconnect();
//			c->send(new datagram(protocol::sysop,"Sysop operation is not allowed."));
			return true;
		}
	}

	if (c->stage==peer_t::sysop) {
		cout << "Connection from myself" << endl;
		delete d;
		c->disconnect();
		return true;
	}

	switch(d->service) {
		case protocol::query_block: {
			process_query_block(c,d); 
			return true;
		}
		case protocol::block: {
			process_block(c,d); 
			return true;
		}
		case protocol::vote_tip: {
			process_vote_tip(c,d); 
			return true;
		}
	}
/*
	if (!auth_app->in_service()) {
		delete d;
		return true;
	}
*/	
	if (!syncdemon.in_sync()) {
		delete d;
		cout << "missing command cause I am syncing" << endl;
		return true;
	}
/*
cout << "Delivering to " << apps_.size() << " apps" << endl;
	bool processed=false;
	for (auto&i:apps_) {
//		if (!i.second->in_service()) continue;
        cout << "delivering to app " << i.second->get_name() << endl;
		if (i.second->process_work(c,d)) {
			processed=true;
			break;
		}
	}
//cout << "d" << endl;

	if (processed) return true;
*/
	switch(d->service) {
		case protocol::local_deltas: {
			process_incoming_local_deltas(c,d); 
			return true;
		}
	}
	


	return false;
}

/*
void c::clear_db() {
	lock_guard<mutex> lock(mx_import); 
	for (auto&i:apps_) {
		i.second->clear_db();
	}
	set_last_block_imported_("");
}
*/

void c::set_last_block_imported_(const diff::hash_t& h) {
	last_block_imported=h;
	string filename=blocksdir()+"/head";
	ofstream os(filename);
	os << last_block_imported << endl;
}

void c::clear() {
	cout << "DB cleared" << endl; //TODO
	for (auto&i:apps_) {
        i.second->clear();
	}
	set_last_block_imported(0);
}


void c::set_last_block_imported(const diff::hash_t& h) {
	lock_guard<mutex> lock(mx_import); 
	set_last_block_imported_(h);
}

bool c::import(const diff& b) {
        lock_guard<mutex> lock(mx_import); 
	cout << "blockchain: importing diff " << b.hash() << endl;

        if (b.prev!=last_block_imported) {
                cout << "block not in sequence." << b.prev << " " << last_block_imported << endl;
                return false;
        }
/*  
	lock_guard<mutex> lock(mx_import); 
	if (b.base!=dbhash()) {
		cout << "block not in sequence" << endl;
		return false;
	}
*/
	app::last_block_imported=last_block_imported;

	for (auto&i:b) {
		auto a=apps_.find(i.first);
		assert(a!=apps_.end());
		assert(a->second!=0);
		a->second->import(*i.second,b.proof_of_work);
		if (a->second==auth_app) {
			update_peers_state();
		}
		//for (auto& g: b) {
		//	auto ag=g.second->get_app_gut(i.second->get_id());
		//	if (ag!=0) i.second->import(*ag);
		//}
		//i.second->import(*ag);
		//auto* cg=i.second->create_closure_gut(b);
		//if (cg!=0) i.second->import(*cg);
	}
	set_last_block_imported_(b.hash());
	//syncdemon.update(b.hash()); //tail

	//cached_dbhash_ok=false;

	return true;

//return false;
}


void c::votes_t::clear() {
	lock_guard<mutex> lock(mx);
	b::clear();
}
bool c::votes_t::add(const pubkey_t::hash_t& h,const diff::hash_t& v) {
	auto i=find(h);
	if (i!=end()) {
		++i->second.second;
		return false; //not new to me
	}
	emplace(h,make_pair(v,1));
	return true;
}

diff::hash_t c::votes_t::select() {
	lock_guard<mutex> lock(mx);
	if (empty()) return diff::hash_t(0);
	unsigned long max{0};
	diff::hash_t* hash;
	for (auto&i:*this) {
		if (i.second.second>=max) {
			max=i.second.second;
			hash=&i.second.first;
cout << "xx " << i.second.first << endl;
		}
	}
	auto ans=*hash;
	b::clear();
	return move(ans);
}

bool c::sysops_t::process_work(peer_t *p, datagram*d) {
	if (d->service!=protocol::sysop) return false;
cout << "processing sysop data request " << d->parse_string() << endl;
	lock_guard<mutex> lock(mx); 
	auto i=find(p);
	if (i==end()) {
		i=emplace(p,shell(this->d)).first;
	}
	string response=i->second.command(d->parse_string());
	delete d;
cout << "sending response: " << response << endl;
	p->send(us::gov::protocol::sysop,response);
	return true;
}

string c::apps::shell_command(int app_id, const string& cmdline) const {
	auto i=find(app_id);
	if (i==end()) return "app not found";
	return i->second->shell_command(cmdline);
}

string c::shell_command(int app_id, const string& cmdline) const {
	return apps_.shell_command(app_id,cmdline);
}

void c::apps::dump(ostream& os) const {
	for (auto&i:*this) {
		os << i.first << '\t' << i.second->get_name() << endl;
	}
}

void c::list_apps(ostream& os) const {
	apps_.dump(os);
}

void c::dump(ostream& os) const {
	os << "Hello from blockchain::daemon" << endl;

}

