#include "daemon.h"
#include <us/gov/auth.h>
#include "protocol.h"
#include <us/gov/signal_handler.h>

using namespace us::gov::engine;
using namespace std;

typedef us::gov::engine::daemon c;

void c::constructor() {
	auth_app=new auth::app(id.pub);
	add(auth_app);
}

c::daemon(const keys& k): rng(chrono::steady_clock::now().time_since_epoch().count()), peerd(this), sysops(*this), syncd(this), id(k) { //dependency order
	constructor();
}

c::daemon(const keys& k, const string& home, uint16_t port, uint8_t edges, const vector<string>& seed_nodes): rng(chrono::steady_clock::now().time_since_epoch().count()), peerd(port, edges, this, seed_nodes), home(home), syncd(this), sysops(*this), id(k) { //dependency order
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
	apps_.emplace(app->get_id(),app);
//    app->parent=this;
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
//cout << "Updating tail of syncd: " << lbi << endl;
	syncd.update(lbi);
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

void c::eat_diff(const diff::hash_t& voted_tip, cycle_t& cycle) {
		if (likely(cycle.new_diff!=0)) {
			if (likely(voted_tip==cycle.new_diff->hash())) {
				save(*cycle.new_diff);
				if (!import(*cycle.new_diff)) {
			        clear();
					cerr << "DB CLEARED" << endl;
				}
			}
			delete cycle.new_diff;
			cycle.new_diff=0;
		}
		syncd.update(voted_tip,get_last_block_imported()); //head,tail
}

void c::stage1(cycle_t& data) {
//cout << "stage1 NB1 " << data.new_diff << endl;
	if (!data.new_diff) return;
	if (auth_app->my_stage()!=peer_t::node) return;
//cout << "stage1 VOTETIP " << endl;
	vote_tip(*data.new_diff);
//cout << "NB1 " << data.new_diff << endl;
//cout << "/stage1" << endl;
}

void c::stage2(cycle_t& cycle) {
	diff::hash_t tip=votes.select(); //decide tip
    app::chaininfo.set_tip(tip); //tx validation, now they have to refer to this new tip

cout << "Voting process result: diff " << tip << endl;
	if (likely(!tip.is_zero())) {
        eat_diff(tip,cycle);
//		if (cycle.get_stage()!=cycle_t::local_deltas_io) return false; //TODO review
	}
}

bool c::stage3(cycle_t& cycle) {
	if (unlikely(!auth_app->is_node())) return false;
	if (unlikely(!syncd.in_sync())) return false;

	auto* mg=create_local_deltas();
	if (mg!=0) {
		send(*mg);
		{
		lock_guard<mutex> lock(mx_pool);
		if (pool->allow(*mg)) pool->add(mg);
		}
	}
	return true;
}

void c::stage4(cycle_t& cycle) {

	assert(cycle.new_diff==0);
	{
	lock_guard<mutex> lock(mx_pool);
	pool->end_adding();
	cycle.new_diff=pool;
	pool=new diff();
	}
	cycle.new_diff->prev=get_last_block_imported();
}

void c::load_head() {
        string filename=blocksdir()+"/head";
        ifstream is(filename);
        diff::hash_t head(0);
        if (is.good()) is >> head;
        syncd.update(head,get_last_block_imported());
        app::chaininfo.set_tip(head);
}

void c::evidence_processor() {

//    time_point<steady_clock> dt(nanoseconds(duration));

    static constexpr seconds idle_wakeup {1};
    static constexpr seconds relay_interval {5};
    uint64_t last_ev_ts=duration_cast<nanoseconds>((system_clock::now()-relay_interval).time_since_epoch()).count();

    int max=0;
    int rst=0;

    while(!program::_this.terminated) {
            evidence* e;
            app* p;
            {
            unique_lock<mutex> lock(calendar.mx);
            if (++rst>300) { max=0; rst=0; }
            if (calendar.size()>max) max=calendar.size();
            
            evidences_on_hold.wait_empty();

            auto i=calendar.begin();
            if (i==calendar.end()) {
        lock.unlock();
                thread_::_this.sleep_for(idle_wakeup);
                continue;
            }
            uint64_t tip_ts=duration_cast<nanoseconds>((system_clock::now()-relay_interval).time_since_epoch()).count();
            if (i->first>tip_ts) {
        lock.unlock();
            cout << "evidences calendar max size: " << max << endl;
                thread_::_this.sleep_for(idle_wakeup);
                continue;
            }
            e=i->second.first;
            p=i->second.second;
//            cout << "calendar consumer - size: " << t->inputs.size() << endl;
            calendar.erase(i);
            }
            if (e->ts<last_ev_ts) { //enforce tx order
                cout << "deleting ev with ts " << e->ts << " older than last_ev_ts " << last_ev_ts << endl;
                delete e;
                continue;
            }
//            cout << " tx@" << t->ts << endl;
            p->process(*e);
            last_ev_ts=e->ts;
            delete e;
    }

}

void c::run() {
	//if (miners_size()>1000) peerd.set_ip4(); else peerd.set_tor();
	thread peers(&networking::run,&peerd);
    thread tep(&c::evidence_processor, this);

	vector<thread> apps_threads;
	apps_threads.reserve(apps_.size());
	for (auto& i:apps_) {
		auto p=dynamic_cast<runnable_app*>(i.second);
		if (p!=0) apps_threads.emplace_back(thread(&runnable_app::run,p));
	}

	signal_handler::_this.add(&syncd);

	thread synct(&syncd_t::run,&syncd);

cout << "it looks stupid I am waiting for 5 secs now" << endl;
	thread_::_this.sleep_for(chrono::seconds(5));

	assert(pool==0);
	pool=new diff();

	load_head();

	while(!program::_this.terminated) { // main loop
		cycle.wait_for_stage(cycle_t::new_cycle);
		stage1(cycle);

		cycle.wait_for_stage(cycle_t::local_deltas_io);
		stage2(cycle);

		cycle.wait_for_stage(cycle_t::sync_db);
		if (!stage3(cycle)) continue;

		cycle.wait_for_stage(cycle_t::consensus_vote_tip_io);
		stage4(cycle);
	}

	synct.join();
	for (auto& i:apps_threads) i.join();
    tep.join();
	peers.join();
}

#include <us/gov/crypto/base58.h>
#include <fstream>

string c::blocksdir() const {
    return home+"/blocks";
}

#include <us/gov/stacktrace.h>

void c::save(const diff& bl) const {
	ostringstream fn;
	fn << blocksdir()+"/"+bl.hash().to_b58();
	{
	ofstream os(fn.str());
	bl.to_stream(os);
	}
#ifdef DEBUG
cout << "------------SAVE CHECK - DEBUG MODE------------" << "file " << fn.str() << endl;
	if (!file_exists(fn.str())) {
		cerr << "file should be in the filesystem, I just saved it" << endl;
		print_stacktrace();
		assert(false);
	}
	ifstream is(fn.str());
	if (!is.good()) {
		cerr << "file should be good in the filesystem, I just saved it" << endl;
		print_stacktrace();
		assert(false);
	}
	diff*b=diff::from_stream(is);
	if (!b) {
		cout << "ERROR A" << endl;
		print_stacktrace();
		assert(false);
	}
	if (b->hash()!=bl.hash()) {
		cout << b->hash() << " " << bl.hash() << endl;
		{
		ofstream os(blocksdir()+"/"+bl.hash().to_b58()+"_");
		b->to_stream(os);
		}
		cout << "ERROR B " << (blocksdir()+"/"+bl.hash().to_b58()+"_") << endl;
		print_stacktrace();
		assert(false);
	}
	delete b;
#endif
}

string c::get_random_node(const unordered_set<string>& exclude_addrs) const {
	auto s=auth_app->get_random_node(rng,exclude_addrs);
//cout << "get rnd node from authapp " << s << endl;
	return s;
}

peer_t* c::query_block(const diff::hash_t& hash) {
	auto n=get_random_edge();
	if (unlikely(n==0)) return n;
//	cout << "querying diff " << hash << " to " << n->pubkey << endl;
	auto r=n->send(new datagram(protocol::query_block,hash.to_b58()));
	if (unlikely(!r.empty())) {
		cerr << "delivery failed. " << r << endl;
		return 0;
	}
	return n;
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
	return true;
}

peer_t* c::get_random_edge() {
    auto n=get_people(); //sysops are not taken into account
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
        auto p=static_cast<peer_t*>(i);
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
		if (p->stage!=peer_t::sysop) {
			//for (int i=0; i<sizeof(p->pubkey.data); ++i)
			//	cout << p->pubkey.data[i]  << " ";
			//cout << endl;
			p->stage=auth_app->db.get_stage(p->pubkey.hash());
		}
	}
}

void c::send(const local_deltas& g, peer_t* exclude) {
	ostringstream os;
	g.to_stream(os);
	string msg=os.str();
    datagram d(protocol::local_deltas,msg); //TODO optimize, write directly in the datagram
	for (auto& i:peerd.get_nodes()) {
		if (i==exclude) continue; //dont relay to the original sender
		i->send(d);
	}
}

local_deltas* c::create_local_deltas() {
//	cout << "blockchain: create_local_deltas" << endl;
	auto* mg=new local_deltas();

	{
    lock_guard<mutex> lock(peerd.mx_evidences);
	for (auto&i:apps_) {
		auto* amg=i.second->create_local_delta(); //
		if (amg!=0) {
			mg->emplace(i.first,amg);
		}
	}
	auto e=peerd.retrieve_evidences();
    delete e;
	}

	if (mg->empty()) {
		delete mg;
		return 0;
	}
	mg->sign(id);
	return mg;
}

void c::vote_tip(const diff& b) {
	const diff::hash_t& h=b.hash();
	votes.add(id.pub.hash(),h);

cout << "voting for tip: " << h << endl;
	string signature=crypto::ec::instance.sign_encode(id.priv,h.to_b58());
	ostringstream os;
	os << h << " " << id.pub << " " << signature;
	string msg=os.str();
	for (auto& i:get_people()) {
		i->send(new datagram(protocol::vote_tip,msg));
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
				i->send(new datagram(protocol::vote_tip,s));
			}
		}
	}

}
#include <us/gov/likely.h>
void c::process_incoming_local_deltas(peer_t *c, datagram*d) {
	auto s=d->parse_string();
	delete d;
	istringstream is(s);
	local_deltas* g=local_deltas::from_stream(is);
	if (!g) return;
	if (unlikely(!g->verify())) { /// discard if not properly signed
		delete g;
		cout << "Failed verification of incoming local_deltas" << endl;
		return;
	}

	bool allowed;
	{
	lock_guard<mutex> lock(mx_pool);
	allowed=pool->allow(*g);
	}
	if (!allowed) { /// If it is not the first time we receive content from this node discard it
		delete g;
		return;
	}
	send(*g,c); //relay, dont relay to the original sender

	{
	lock_guard<mutex> lock(mx_pool);
	pool->add(g);
	}

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
	c->send(new datagram(protocol::block,content));
}

void c::process_block(peer_t *c, datagram*d) {
	auto content=d->parse_string();
	delete d;
	istringstream is(content);
	diff* b=diff::from_stream(is);
	if (unlikely(b==0)) return;
	cout << "Received content of block " << b->hash() << " from " << c->addr << endl;

	string filename=blocksdir()+"/"+b->hash().to_b58();
	if (!file_exists(filename)) { //TODO write under different name and then rename atomically
//		cout << "Saving it to disk " << filename << endl;
		{
		ofstream os(filename);
		os << content;
		}
//		cout << "waking up sync demon after saving the block" << endl;
//		syncdemon.update();
		syncd.signal_file_arrived();
	}
//	else {
//		cout << "File exists, ignoring. " << filename << endl;
//	}
	delete b;
}

void c::flush_evidences_on_hold() {
    unique_lock<mutex> lock(evidences_on_hold.mx);
    auto i=evidences_on_hold.begin();
    while (i!=evidences_on_hold.end()) {
        auto p=*i;
        lock.unlock();
        if (!process_evidence(p)) {
            cerr << "Nobody recognized this evidence" << endl;
            p->dump(cout);
            delete p;
        }
        lock.lock();
        i = evidences_on_hold.erase(i);
    }

    lock.unlock();
    evidences_on_hold.cv.notify_all();
}

void c::on_sync() { //while syncing evidences are queued in a separate containew evidences_on_hold
    flush_evidences_on_hold();

//    cycle.in_sync=true;
}

bool c::process_evidence(datagram*d) {
    if (!syncd.in_sync()) {
  		cout << "blockchain: holding evidence until sync is completed" << endl;
        evidences_on_hold.add(d);
	    return true;
	}

//	send(*d, c); //relay TODO - check relay daemon
    assert(syncd.in_sync());
//  cout << "blockchain: process evidence" << endl;
    string payload=d->parse_string();
    auto service = d->service;
    delete d;
    
	bool processed=false;
	for (auto&i:apps_) {
        auto e = i.second->parse_evidence(service, payload);
		if (e != 0) {
			processed=true;
            calendar.schedule(e,i.second);
            break;
		}
	}
	return processed;
}

bool c::process_app_query(peer_t *c, datagram*d) {
//cout << "BLOCKCHAIN: process_query " << d->service << endl;
/*
    if (!syncdemon.in_sync()) {
        c->send(new datagram(us::gov::protocol::error,"Service temporarily unavailable. Syncing."));
 //		cout << "ignoring query, I am syncing" << endl;
		delete d;
		return true;
	}
*/
	bool processed=false;
	for (auto&i:apps_) {
		if (i.second->process_query(c,d)) {
			processed=true;
			break;
		}
	}
	return processed;
}


bool c::process_sysop_request(peer_t *c, datagram*d) {
		bool alowed_sysop=sysop_allowed;
		if (c->stage==peer_t::sysop) { //Is peer a sysop?
			if (!sysop_allowed) {
				delete d;
				//c->send(new datagram(protocol::sysop,"Sysop operation is not allowed."));
				c->disconnect();
				return true;
			}
			if (!sysops.process_work(c, d)) { //traslate the msg to sysopland
				cerr << "Error , sysop request not handled" << endl;
				delete d;
			}
			return true;
		}
		delete d;
		c->disconnect();
//		c->send(new datagram(protocol::sysop,"Sysop operation is not allowed."));
		return true;
}




bool c::process_work(peer_t *c, datagram*d) {
/*
	if (unlikely(d->service==protocol::sysop)) {
		if (likely(process_sysop_request(c,d))) return true;
	}

	if (unlikely(c->stage==peer_t::sysop)) { //only service sysop can be operated by a sysop
		cout << "Sysop connection sending invalid datagrams." << endl;
		delete d;
		c->disconnect();
		return true;
	}
*/
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
                case protocol::local_deltas: {
                        process_incoming_local_deltas(c,d);
                        return true;
                }
		case protocol::sysop: {
        	  if (likely(c->stage==peer_t::sysop)) { //only service sysop can be operated by a sysop
	                if (likely(process_sysop_request(c,d))) {
				return true;
			}
                  }
	          else {
        		cout << "unexpected sysop datagram." << endl;
		        delete d;
	        	c->disconnect();
			return true;
            	  }
		}
	}
	return false;
}

void c::set_last_block_imported_(const diff::hash_t& h) {
	last_block_imported=h;
	string filename=blocksdir()+"/head";
	ofstream os(filename);
	os << last_block_imported << endl;
}

void c::clear() {
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
//	cout << "blockchain: importing diff " << b.hash() << endl;
    if (b.prev!=last_block_imported) {
       cout << "block not in sequence." << b.prev << " " << last_block_imported << endl;
       return false;
    }

//    unique_lock<mutex> lock(app::mx_last_block_imported);
	app::chaininfo.last_block_imported=last_block_imported;

	for (auto&i:b) {
		auto a=apps_.find(i.first);
		assert(a!=apps_.end());
		assert(a->second!=0);
		a->second->import(*i.second,b.proof_of_work);
		if (a->second==auth_app) {
			update_peers_state();
		}
	}
	set_last_block_imported_(b.hash());
	return true;
}


void c::votes_t::clear() {
	lock_guard<mutex> lock(mx);
	b::clear();
}

bool c::votes_t::add(const pubkey_t::hash_t& h,const diff::hash_t& v) {
//cout << "Adding vote " << h << endl;
	auto i=find(h);
	if (i!=end()) {
		return false; //not new to me
	}
	emplace(h,make_pair(v,1));
	return true;
}

diff::hash_t c::votes_t::select() {
	map<diff::hash_t,unsigned long> x;
	{
	lock_guard<mutex> lock(mx);
	if (empty()) return diff::hash_t(0);
	for (auto&i:*this) {
		auto a=x.find(i.second.first);
		if (a!=x.end()) {
			a->second++;
//			cout << "VOTE count for " << i.second.first << " is " << a->second << endl;
		}
		else {
			x.emplace(i.second.first,1);
//			cout << "VOTE count for " << i.second.first << " is 1" << endl;
		}
	}
	}

	unsigned long max{0};
	const diff::hash_t* hash;
	for (auto&i:x) {
		if (i.second>=max) {
			max=i.second;
			hash=&(i.first);
		}
	}
	auto ans=*hash;
	b::clear();
	return move(ans);
}

bool c::sysops_t::process_work(peer_t *p, datagram*d) {
	if (d->service!=protocol::sysop) return false;
//cout << "processing sysop data request " << d->parse_string() << endl;
	lock_guard<mutex> lock(mx);
	auto i=find(p);
	if (i==end()) {
		i=emplace(p,shell(this->d)).first;
	}
	string response=i->second.command(d->parse_string());
	delete d;
//cout << "sending response: " << response << endl;
	p->send(new datagram(us::gov::protocol::sysop,response));
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
	os << "Hello from engine::daemon" << endl;
	os << "networking" << endl;
	peerd.dump(os);

}
string c::timestamp() const {
	return "NOW"; //TODO
}

void c::print_performances(ostream& os) const {
	os << "US node " << id.pub << " " << timestamp() << endl;
	os << "Resources" << endl;
	os << "  Network" << endl;
	os << "  Storage" << endl;
	os << "  CPU" << endl;
	os << "Time series by indicator" << endl;
	os << "  DiffHash Histogram" << endl;
	os << "       current: #1st-80%, #2nd-05%" << endl;
	os << "        1m ago: #1st-80%, #2nd-05%" << endl;
	os << "        2m ago: #1st-80%, #2nd-05%" << endl;
	os << "        3m ago: #1st-80%, #2nd-05%" << endl;
	os << "        4m ago: #1st-80%, #2nd-05%" << endl;
	os << "        5m ago: #1st-80%, #2nd-05%" << endl;
	os << "  51% vote power attack" << endl;
	os << "       current: 0%" << endl;
	os << "        1m ago: 0%" << endl;
	os << "        2m ago: 0%" << endl;
	os << "        3m ago: 0%" << endl;
	os << "        4m ago: 0%" << endl;
	os << "        5m ago: 0%" << endl;
	os << "  Missed tip" << endl;
	os << "       current: No" << endl;
	os << "        1m ago: No" << endl;
	os << "        2m ago: No" << endl;
	os << "        3m ago: No" << endl;
	os << "        4m ago: Yes" << endl;
	os << "        5m ago: No" << endl;
}
