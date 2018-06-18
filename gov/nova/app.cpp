#include "app.h"
//#include "peer.h"
#include "protocol.h"
#include <thread>
#include <chrono>
#include <us/gov/crypto/base58.h>
#include <us/gov/likely.h>

typedef us::gov::nova::app c;
using namespace us::gov;
using namespace us::gov::nova;
using namespace std;

constexpr const char* c::name;
//constexpr spend_code_t no_spend_code;
//min_spend_code

//atomic<uint64_t> tx::next_localuid{0};
constexpr array<const char*,app::policies_traits::num_params> app::policies_traits::paramstr;


namespace std {

  template <>
  struct hash<us::gov::nova::app::local_delta> {
	size_t operator()(const us::gov::nova::app::local_delta&) const;
  };

}


size_t std::hash<us::gov::nova::app::local_delta>::operator() (const us::gov::nova::app::local_delta&g) const {
	return *reinterpret_cast<const size_t*>(&g.get_hash()[0]); //TODO LE endian
}
/*
bool c::local_delta::compartiments_t::add_input(tx& t, const hash_t& addr, const cash_t& amount) {
//	spend_code_t sc=lookup_spend_code(addr);
//	if (sc==0) return false;
	cash_t prev_balance=0; //TODO
	return t.add_input(addr,prev_balance, amount);
}
bool c::local_delta::compartiments_t::add_output(tx& t, const hash_t& addr, const cash_t& amount, const hash_t& locking_program) {
	return t.add_output(addr,amount,locking_program);
}

*/

int c::local_delta::app_id() const {
	return app::id();
}

c::app() {
	pool=new c::local_delta();
	policies_local=policies;
}

c::~app() { 
	delete pool;
}
void c::add_policies() {
	lock_guard<mutex> lock(mx_pool);
	lock_guard<mutex> lock2(mx_policies);
	for (int i=0; i<policies_traits::num_params; ++i)
		(*pool)[i] = policies_local[i];
}

blockchain::app::local_delta* c::create_local_delta() {
	cout << "app: nova: create_local_delta " << endl; // << buffer.size() << " verified/unverified txs in mempool. ";
	add_policies();
	lock_guard<mutex> lock(mx_pool);
	auto full=pool;
	pool=new c::local_delta();
	return full; //send collected transactions to the network
}

bool c::process_query(peer_t *c, datagram*d) {
cout << "SGT-01-RECEIVED QUERY" << endl; //settlement go throught
	switch(d->service) {
		case protocol::nova_compartiment_query: {
			compartiment_query(c,d);
			return true;
		} break;
	}
	return false;
}

bool c::process_evidence(peer_t *c, datagram*d) {
cout << "SGT-01-RECEIVED EVIDENCE" << endl; //settlement go throught
	switch(d->service) {
		case protocol::nova_evidence_load: {
			string payload=d->parse_string();
cout << "SGT-01-NOVA TX MOVE " << payload << endl; 
			delete d;
			evidence_load t=evidence_load::from_b58(payload);
t.write_pretty(cout);
			process(t);
			return true;
		} break;
		case protocol::nova_evidence_track: {
			string payload=d->parse_string();
cout << "SGT-01-NOVA TX TRACK " << payload << endl; 
			delete d;
			evidence_track t=evidence_track::from_b58(payload);
t.write_pretty(cout);
			process(t);
			return true;
		} break;
	}
	return false;
}

c::query_compartiments_t c::query_compartiments_t::from_string(const string&s) {
	int n;
	istringstream is(s);
	is >> n;
	query_compartiments_t r;
	r.reserve(n);
	for (int i=0; i<n; ++i) {
		hash_t v;
		is >> v;
		r.emplace_back(move(v));
	}
	return move(r);
}

c::query_compartiments_t c::query_compartiments_t::from_datagram(datagram*d) {
	string query=d->parse_string();
	delete d;
	return from_string(query);
}

void c::compartiment_query(peer_t *c, datagram*d) {
cout << "COMPARTIMENT_QUERY" << endl;
	query_compartiments_t q=query_compartiments_t::from_datagram(d);

	ostringstream os;
	os << "0 "; //ret code ok
	{
	lock_guard<mutex> lock(db.mx);
	for (auto&addr:q) {
		auto a=db.compartiments->find(addr);
		if (likely(a!=db.compartiments->end())) {
			a->second.to_stream(os);
		}
		else {
			compartiment_t a;
			a.locking_program=0;
			//a.balance=0;
			a.to_stream(os);
		}
	}
	}

	os << ' ' << last_block_imported;

cout << "NOVA_RESPONSE " << os.str() << endl;

	c->send(protocol::nova_response,os.str());	
}
/*
bool c::process_work(peer_t *c, datagram*d) {
	switch(d->service) {
		default: return false;
	}
	return true;
}
*/
/*
c::buffer_t::~buffer_t() {
	for (auto i:*this) delete i.second; 
}

bool c::buffer_t::add(tx*t) {
	lock_guard<mutex> lock(mx);
	if (index.find(t->localuid)!=index.end()) { delete t; return false; }
	emplace(t->fee(),t);
	index.emplace(t->localuid);
	return true;
}

void c::buffer_t::set_unverified() {
	lock_guard<mutex> lock(mx);
	for (auto i:*this) i.second->verified=false;
}

tx* c::buffer_t::next_unverified() {
	lock_guard<mutex> lock(mx);
	for (auto i=rbegin(); i!=rend(); ++i)
		if (!i->second->verified) return i->second;
	return 0;
}

void c::buffer_t::destroy(tx* t) {
	lock_guard<mutex> lock(mx);
	for (auto i=begin(); i!=end(); ++i)
		if (i->second==t) { delete t; index.erase(index.find(t->localuid)); erase(i); break; }
}

app_gut* c::buffer_t::gut() {
	auto* g=new app_gut();
	b remaining;
	lock_guard<mutex> lock(mx);
	for (auto i=rbegin(); i!=rend(); ++i)
		if (i->second->verified) g->push_back(i->second);
		else remaining.emplace(*i);
	static_cast<b>(*this)=move(remaining);
	index.clear();
	for (auto i:*this) { index.emplace(i.second->localuid); }
	return g;
}

void c::verification_daemon() {
	using namespace chrono_literals;
	while(!thread_::_this.terminated) {
		auto t=buffer.next_unverified();
		if (t==0) {
			thread_::_this.sleep_for(300ms);
		}
		else {
			validation_result r=validate(*t);
			if (r==valid) t->verified=true; 
			else if (r==invalid) buffer.destroy(t);
		}
	}
}
*/
void c::run() {
	while(!program::_this.terminated) {
//		cout << "APP: cash: doing something" << endl;
		db.dump(cout);

		thread_::_this.sleep_for(chrono::seconds(10));
		//parent->peerd.

	}
}
/*
void c::compartiments_t::p2pkh_t::dump(ostream& os) const {
	cout << size() << " p2pkh compartiments:" << endl;
	for (auto& i:*this) {
		cout << ' ' << i.first << ' ' << i.second.second << " (spend-code " << i.second.first << ")" << endl;
	}
}
void c::compartiments_t::stdtx_t::dump(ostream& os) const {
	cout << size() << " stdus compartiments:" << endl;
	for (auto& i:*this) {
		cout << ' ' << i.first << ' ' << i.second.first << ' ' << i.second.second.second << " (spend-code " << i.second.second.first << ")" << endl;
	}
}
void c::compartiments_t::p2sh_t::dump(ostream& os) const {
	cout << size() << " p2sh compartiments:" << endl;
	for (auto& i:*this) {
		cout << ' ' << i.first << ' ' << i.second.first << ' ' << i.second.second.second << " (spend-code " << i.second.second.first << ")" << endl;
	}
}
*/

void c::local_delta::compartiment_t::dump(ostream& os) const {
 os << "locking_program " << locking_program << "; data:" << endl;
 logbook.dump(os); // << "; spend code " << spend_code;
}

void c::local_delta::compartiments_t::dump(ostream& os) const {
	cout << size() << " compartiments:" << endl;
	for (auto& i:*this) {
		cout << ' ' << i.first << ' ';
		i.second.dump_brief(os);
		//os << endl;
	}
/*
	p2pkh.dump(os);
	stdtx.dump(os);
	p2sh.dump(os);
*/
}

void c::db_t::dump(ostream& os) const {
	cout << "nova app db dump" << endl;
	lock_guard<mutex> lock(mx);
	compartiments->dump(os);
	//cout << "supply_left " << supply_left << " block_reward " << block_reward << endl;
}

void c::db_t::to_stream(ostream&os) const {
	compartiments->to_stream(os);
	//os << ' ';
	//os << supply_left << ' ' << block_reward;
}

c::db_t c::db_t::from_stream(istream& is) {
	db_t db;
	db.compartiments=compartiments_t::from_stream(is);
	//is >> db.supply_left;
	//is >> db.block_reward;
	return move(db);
}

/*
#include <limits>
spend_code_t c::compartiments_t::new_spend_code(const spend_code_t& prev) const {
	if (unlikely(prev==numeric_limits<spend_code_t>::max())) return 0;
	return prev+1;
}
*/
/*
c::spend_code_t c::compartiments_t::new_spend_code(unsigned int seed) const {
	default_random_engine generator(seed);
	uniform_int_distribution<spend_code_t> distribution(0,numeric_limits<spend_code_t>::max());
	return distribution(generator);
}
*/
/*
bool c::local_delta::compartiments_t::pay(const hash_t& address, const cash_t& amount) { //caller has to get the lock
	if (amount<=0) return false;
	auto i=find(address);
	if (i==end()) {
		emplace(make_pair(address,compartiment_t{1,amount}));
	}
	else {
		//same spend_code
		i->second.balance+=amount;
	}
	return true;
}
*/


//void c::compartiments_t::rollback(undo_t& undo) {
/*
	for (auto i=undo.rbegin(); i!=undo.rend(); ++i) {
		int code=get<0>(*i);
		const hash_t& h=get<1>(*i);
		const spend_code_t& sc=get<2>(*i);
		const cash_t& a=get<3>(*i);
		if (code==1) { //insert
			p2pkh.emplace(h,make_pair(sc,a));
		}
		else if (code==0) { //update
			auto i=p2pkh.find(h);
			i->second.first=sc;
			i->second.second=a;
		}
	}
*/
//}



/*
bool c::local_delta::compartiments_t::withdraw(const hash_t& address, const cash_t& amount) { //caller has to get the lock
	auto i=find(address);
	if (i==end()) {
		return false;
	}
	else if (i->second.balance<amount) {
		return false;
	}

		//locate the output;
	i->second.balance-=amount;
	if (unlikely(i->second.balance==0)) {
		erase(i);
	}
	return true;
}
*/

/*
bool c::db_t::add_(const hash_t& k, const cash_t& amount) { //caller has to get the lock
	return compartiments->pay(k,amount);
}


bool c::db_t::withdraw_(const hash_t& k, const cash_t& amount) { //caller has to get the lock
	return compartiments->withdraw(k,amount);
}
*/

/*
bool c::db_t::move(const tx::end_t& srcs, const tx::end_t& dsts) {
	vector<pair<compartiments_::iterator,uint64_t>> inputs;
	vector<pair<compartiments_::iterator,uint64_t>> outputs;
	uint64_t sum_inputs=0;
	uint64_t sum_outputs=0;
	lock_guard<mutex> lock(mx);
	for (auto& src:srcs) {
		auto s=compartiments.find(src.first);
		if (s==compartiments.end()) return false;
		if (s->second<src.second) return false;
		inputs.push_back(make_pair(s,src.second));
		sum_inputs+=src.second;
	}
	vector<pair<string,uint64_t>> new_outputs;
	for (auto& dst:dsts) {
		auto s=compartiments.find(dst.first);
		if (s==compartiments.end()) {
			new_outputs.emplace_back(dst.first,dst.second);
		}
		else {
			s->second+=dst.second;
		}
		sum_outputs+=dst.second;
	}
	//if (sum_outputs>sum_inputs) return false;  //coinbase breaks this
	for (auto& i: inputs) {
		i.first->second-=i.second;
		if (i.first->second==0) compartiments.erase(i.first);
	}
	for (auto& i: outputs) {
			i.first->second+=i.second;
	}
	for (auto& i: new_outputs) {
		compartiments.emplace(i.first,i.second);
	}
	return true;
}

void c::clear_db() {
	db.clear();
}

bool c::db_t::move(const string& src, const string& dst, uint64_t amount) {
	lock_guard<mutex> lock(mx);
	auto s=compartiments.find(src);
	if (s==compartiments.end()) return false;
	if (s->second<amount) return false;
	if (s->second==amount) 
		compartiments.erase(s);
	else 
		s->second-=amount;
	auto d=compartiments.find(dst);
	if (d==compartiments.end()) {
		compartiments.emplace(dst,amount);
	}
	else {
		d->second+=amount;
	}
	return true;
}

void c::db_t::clear() {
	lock_guard<mutex> lock(mx);
	compartiments.clear();
}
*/
/*
double c::supply_function(double x0, double x, double xf) const {
	static constexpr double total_supply=21e14;
	static constexpr double initial_block_reward=100e8;
	double t=(x-x0)/(4*365*24*60*60); //number of periods elapsed since the beginning.  //T=4 years 4*365*24*60*60
	if (t<0) return 0;
0 /2
1 /2*2
2 /2*2*2
3 /2*2*2*2
	
	double p=(x-x0)/60; //number of minutes elapsed since the beginning.  //5 every 1 minute
	if (p<=0 || p>=1) return 0;
	return total_supply*p;
}
*/
/*
cash_t c::db_t::get_newcash() { //db lock must be acquired
	if (likely(supply_left>=block_reward)) {
		supply_left-=block_reward;
	}
	else {
		block_reward=supply_left;
		supply_left=0;
	}
	return block_reward;
}
*/
void c::import(const blockchain::app::delta& gg, const blockchain::pow_t& w) {
cout << "nova: importING appgut2 MULTIPLICITY " << gg.multiplicity << endl;
	const delta& g=static_cast<const delta&>(gg);
	{
	lock_guard<mutex> lock(mx_policies);
	for (int i=0; i<policies_traits::num_params; ++i) policies[i]=g[i];
	}

//patch db.

	{
	const c::local_delta::compartiments_t& a=g.g.compartiments;
	lock_guard<mutex> lock(db.mx);
	for (auto& i:a) {
//cout << "AAAAAAAAAAAAAAAAA- " << i.first << endl;

		auto d=db.compartiments->find(i.first);
		if (d==db.compartiments->end()) {
			//if (likely(i.second.balance!=0)) {
//cout << "AAAAAAAAAAAAAAAAAA " << i.second.locking_program << endl;
			db.compartiments->emplace(i);
			//}
		}
		else {
//			if (i.second.balance==0) {
//cout << "AAAAAAAAAAAAAAAAA3" << endl;
//				db.compartiments->erase(d);
//			}
//			else {
//cout << "AAAAAAAAAAAAAAAAA2 " << i.second.locking_program << endl;
				d->second=i.second;
//				d->second.balance=i.second.balance;
//			}
		}
	}

	}

//spread fees
/*
	if (likely(!w.empty())) {
	///spread newcash+fees weighted by work
		lock_guard<mutex> lock(db.mx);
		cash_t newcash=db.get_newcash();
		cash_t total_cash_int=newcash+g.g.fees;
cout << "total_cash_int " << total_cash_int << " " << g.g.fees << endl;
		double total_cash=(double)total_cash_int;
		uint64_t total_work=w.sum();
		cash_t paid=0;
		for (auto&i:w) {
cout << "PoW " << i.first << " " << i.second << endl;
			double rel_work;
			if (likely(total_work>0)) {
				rel_work=(double)i.second/(double)total_work;
			}
			else {
				rel_work=1.0/(double)w.size();
			}
cout << "rel_work " << rel_work << endl;
			cash_t amount=total_cash*rel_work;
cout << "amount " << amount << endl;
			db.add_(i.first,amount);
			paid+=amount;
		}
cout << "paid " << paid << endl;
		cash_t remainder=total_cash_int-paid;
cout << "remainder " << remainder << endl;
		if (remainder>0) {
			default_random_engine generator(get_seed());
			uniform_int_distribution<size_t> distribution(0,w.size()-1);
			auto i=w.begin();
			advance(i,distribution(generator));
			cout << "lucky guy " << i->first << " got remainder " << remainder << endl;
			db.add_(i->first,remainder);
		}
	}
*/
}

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#include "locking_programs/single_signature.h"

bool c::unlock(const hash_t& compartiment, const hash_t& locking_program, const string& locking_program_input, const evidence& e) {
	if (likely(locking_program<min_locking_program)) {
		if (unlikely(locking_program==0)) {
			return true;
		}
		else if (locking_program==single_signature::locking_program_hash) {
			return single_signature::check_input(compartiment, e, locking_program_input);
		}
	}

	//check https://en.wikipedia.org/wiki/AppArmor
	ostringstream prog;
	prog << "--private /home/mmayorga/.bitcoin++/locking/" << locking_program << "/main";
//	prog << "firejail --private ";
	pid_t pid = fork();
	switch (pid) {
		case -1: /* Error */
		    cerr << "Uh-Oh! fork() failed.\n";
		    exit(1);
		case 0: /* Child process */
		    cout << "firejail " << prog.str().c_str() << "\n";
		    execl("firejail",prog.str().c_str()); /* Execute the program */
		    cerr << "Uh-Oh! execl() failed!"; /* execl doesn't return unless there's an error */
		    exit(1);
		default: /* Parent process */
		    cout << "Process created with pid " << pid << "\n";
		    int status;

		    while (!WIFEXITED(status)) {
		        waitpid(pid, &status, 0); /* Wait for the process to complete */
		    }
		    std::cout << "Process exited with " << WEXITSTATUS(status) << "\n";
		    return status==0;
	}
}

bool c::fetch_compartiment(const local_delta::batch_t& batch, const hash_t& address, compartiment_t& acc) const {
cout << "SGT-03-Acc state " << " address " << address << endl; 
		auto  b=batch.find(address);
		if (likely(b==batch.end())) { //this input has already been consumed at least once
cout << "SGT-03-Acc state " << " not in batch " << endl; 
			auto p=pool->compartiments.find(address);
			if (likely(p==pool->compartiments.end())) { //ref in db
cout << "SGT-03-Acc state " << " not in pool " << endl; 

				lock_guard<mutex> lock(db.mx);
				auto o=db.compartiments->find(address);
				if (o==db.compartiments->end()) {
cout << "SGT-03-Acc state " << " not in db " << endl; 
					return false;
				}
				acc=o->second;
				//acc.locking_program=o->second.locking_program;
				//acc.balance=o->second.balance;
			}
			else { //ref in pool
cout << "SGT-03-Acc state " << " in pool " << endl; 
				acc=p->second;
				//acc.locking_program=p->second.locking_program;
				//acc.balance=p->second.balance;
			}

		}
		else { //ref in batch
cout << "SGT-03-Acc state " << " in batch " << endl; 
/*
			if (b->second.spend_code!=i.spend_code) { //if spend_code does not match the expected one, assume this tx has been already processed
				return false;
			}
*/
			acc=b->second;
//			acc.locking_program=b->second.locking_program;
//			acc.balance=b->second.balance;
		}
		return true;
//cout << "SGT-03-Acc state " << " locking_program  " << acc.locking_program << endl; 
//cout << "SGT-03-Acc state " << " balance  " << acc.balance << endl; 
}

bool c::process(const evidence_load& e) {
	cout << "nova: Processing evidence load " << endl;

	if (e.parent_block!=last_block_imported) {
		cout << "SGT-02-tx.rejected - base mismatch - " << e.parent_block << " != base:" << last_block_imported << endl; 
		return false;
	}

	local_delta::batch_t batch;

	compartiment_t state; 

	if (!fetch_compartiment(batch,e.compartiment,state)) {
        state.locking_program=1;
	}

	if (!unlock(e.compartiment,state.locking_program, e.locking_program_input, e)) {
cout << "SGT-02-tx.  UNABLE TO UNLOCK. denied." << endl; 
		return false;
	}
cout << "SGT-03-tx.  UNLOCKED." << endl; 

    if (e.load) {
		state.logbook.add(e.item);
    }
    else {
		state.logbook.rm(e.item);
    }
	batch.add(e.compartiment,state);

	pool->compartiments.add(batch);
cout << "SGT-02-tx OK" << endl; 
	return true;
}


bool c::process(const evidence_track& e) {
	cout << "nova: Processing evidence track " << endl;

	if (e.parent_block!=last_block_imported) {
		cout << "SGT-02-tx.rejected - base mismatch - " << e.parent_block << " != base:" << last_block_imported << endl; 
		return false;
	}

    if (e.data.empty()) {
		cout << "SGT-02-tx.rejected - empty tracking info " << endl; 
        return false;
    }

	local_delta::batch_t batch;

	compartiment_t state; 

	if (!fetch_compartiment(batch,e.compartiment,state)) {
cout << "SGT-02-ev_load " << " fetch_compartiment returned false.CREATING compartiment " << endl; 
        state.locking_program=1;
//		return false;
	}

//cout << "SGT-02-tx.Input #" << j << " UNLOCK.." << endl; 
	if (!unlock(e.compartiment,state.locking_program, e.locking_program_input, e)) {
		return false;
	}

	state.logbook.push_back(e.data);

	batch.add(e.compartiment,state);

	pool->compartiments.add(batch);
cout << "SGT-02-tx OK" << endl; 
	return true;
}


void c::local_delta::to_stream(ostream& os) const {
	os << compartiments.size() << " ";
	for (auto& i:compartiments) {
		os << i.first << ' '; // << /*i.second.spend_code << ' ' <<*/ i.second.balance << ' ';
		i.second.to_stream(os);
	}
//cout << "appgut WRITE fees: " << fees << endl;
//	os << fees << ' ';
	b::to_stream(os);
}

void c::local_delta::from_stream(istream& is) {
	int n;
	is >> n;
	for (int i=0; i<n; ++i) {
		hash_t pubk;
		is >> pubk;

		batch_item bi=batch_item::from_stream(is);
		compartiments.emplace(move(pubk),move(bi));
	}
//	is >> fees;
//cout << "appgut READ fees: " << fees << endl;
	b::from_stream(is);
}

void c::delta::to_stream(ostream& os) const {
/*
	os << to_hall.size() << " ";
	for (auto& i:to_hall) {
		os << i.first << " " << i.second << " ";
	}
//	os << policies_diff.size() << endl;
	for (auto& i:policies) {
		os << i << " ";
	}
*/
	g.to_stream(os);
	b::b1::to_stream(os);

}

c::delta* c::delta::from_stream(istream& is) {
	delta* g=new delta();
/*
	{
	int n;
	is >> n;
	//to_hall.reserve(n);
	for (int i=0; i<n; ++i) {
		pubkey pkey;
		address addr;
		is >> pkey;
		is >> addr;
		g->to_hall.emplace(make_pair(pkey,addr));
		//auto t=tx::from_stream(is);
		//emplace(t.compute_hash(),make_pair(multiplicity,move(t)));
	}
	}

	{
	for (int i=0; i<policies_traits::num_params; ++i) {
		double v;
		is >> v;
		g->policies[i]=v;
	}
	}
*/
	g->g.from_stream(is);

	static_cast<b*>(g)->from_stream(is);
	return g;
}


/*
void usgov::cash::tx::to_stream(ostream& os) const {
//	os << transition << " " << pubkey << " " << address << endl;
	os << inputs.size() << endl;
	for (auto& i:inputs) {
		os << i.first << " " << i.second << endl;
	}
	os << outputs.size() << endl;
	for (auto& i:outputs) {
		os << i.first << " " << i.second << endl;
	}
}

void usgov::cash::tx::from_stream(istream& is, end_t& dest) {
	size_t n;
	{
	is >> n;
	string line;
	getline(is,line);
	}
	dest.reserve(n);
	for (auto i=0; i<n; ++i) {
		string line;
		string pubk;
		uint64_t am;
		is >> pubk;
		is >> am;
		getline(is,line);
		dest.emplace_back(pubk,am);
	}
}

tx* usgov::cash::tx::from_stream(istream& is) {
	tx*t=new tx();
	from_stream(is,t->inputs);
	from_stream(is,t->outputs);
	return t;
}

uint64_t usgov::cash::tx::fee() const {
	uint64_t ti=0;
	for (auto&i:inputs) ti+=i.second;
	uint64_t to=0;
	for (auto&i:outputs) to+=i.second;
	if (ti<to) return 0;
	return ti-to;
}
*/
string c::shell_command(const string& cmdline) {
	ostringstream os;
	istringstream is(cmdline);
	string cmd;
	is >> cmd;
	if (cmd=="hello") {
		os << "nova shell. type h for help." << endl;
	}
	else if (cmd=="h" || cmd=="help") {
		os << "Nova shell." << endl;
		os << "h|help              Shows this help." << endl;
		os << "p|policies [id vote]          ." << endl;
		os << "mempool           Dumps the mempool." << endl;
		os << "exit                Exits this app and returns to parent shell." << endl;
		os << "" << endl;
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
	else if (cmd=="mempool") {
        os << "mempool:" << endl;
    	lock_guard<mutex> lock(mx_pool);
        pool->compartiments.dump(os);
        
    }
	else if (cmd=="exit") {
	}
	else {
		os << "Unrecognized command" << endl;
	}
	return os.str();
}

void c::db_t::clear() {
	lock_guard<mutex> lock(mx);
    delete compartiments;
	compartiments=new compartiments_t();
//    supply_left=2100000000000000; //21.000.000e8  21e14
//    block_reward=500000000; //5e8
}

void c::dbhash(hasher_t&) const {
assert(false);
}

void c::clear() {
    {
	lock_guard<mutex> lock(mx_policies);
    policies.clear();
    }
    db.clear();
}


void c::dump_policies(ostream& os) const {
	lock_guard<mutex> lock(mx_policies);
	os << policies_traits::num_params << " consensuated variables:" << endl;
	for (int i=0; i<policies_traits::num_params; ++i) {
		os << "  " << i << ". " << policies_traits::paramstr[i] << " [avg] consensus value: " << policies[i] << " local value:" << policies_local[i] << endl;
	}
}










c::local_delta& c::local_delta::operator =(int zero) {
	*this=local_delta();
	return *this;
}

c::local_delta::local_delta() {
}

c::local_delta::~local_delta() {
	//for (auto i:*this) delete i; // delete checkpoint_; 
}

c::local_delta::compartiment_t::compartiment_t() {
}

c::local_delta::compartiment_t::compartiment_t(const hash_t& locking_program, const logbook_t& lb): locking_program(locking_program), logbook(lb) {
}

void c::local_delta::logbook_t::dump_brief(ostream& os) const {
	os << size() << " logentries; " << items.size() << " items" << endl;
}
void c::local_delta::logbook_t::dump(ostream& os) const {
	os << size() << " logentries: " << endl;
    for (auto&i:*this) {
        os << i << endl;
        os << "---" << endl;
    }
	os << items.size() << " items: " << endl;
    for (auto&i:items) {
        os << i << ' ';
    }
    os << endl;

//	os << size() << " logentries; " << items.size() << " items";

}

void c::local_delta::logbook_t::to_stream(ostream& os) const {
    os << size() << ' ';
    for (auto&i:*this) {
        os << i << ' ';
    }
    os << items.size() << ' ';
    for (auto&i:items) {
        os << i << ' ';
    }
}

c::local_delta::logbook_t c::local_delta::logbook_t::from_stream(istream& is) {
    logbook_t ans;

    int n;
    is >> n;
    ans.reserve(n);
    for (int i=0; i<n; ++i) {
        string v;
        is >> v;
        ans.push_back(v);
    }
    is >> n;
    for (int i=0; i<n; ++i) {
        item_t v;
        is >> v;
        ans.items.insert(v);
    }
    return move(ans);
}

void c::local_delta::compartiment_t::to_stream(ostream& os) const {
	os << locking_program << ' ';
    logbook.to_stream(os);
    os << ' ';
}

c::local_delta::compartiment_t c::local_delta::compartiment_t::from_stream(istream& is) {
	compartiment_t i;
	is >> i.locking_program;
    i.logbook=logbook_t::from_stream(is);
	return move(i);
}

void c::local_delta::batch_t::add(const hash_t& address, const batch_item& bi) {
	auto i=find(address);
	if (likely(i==end())) {
		emplace(address,bi);
	}
	else {
		i->second=bi;
	}
}

void c::local_delta::compartiments_t::add(const batch_t& batch) {
	for (auto& b:batch) {
		auto i=find(b.first);
		if (likely(i==end())) {
			emplace(b);
		}	
		else {
			//i->second.spend_code=b.second.spend_code;
			i->second=b.second;
		}
	}
}
/*
cash_t c::local_delta::compartiments_t::get_balance() const {
	cash_t b=0;
	for (auto&i:*this) {
		b+=i.second.balance;
	}
	return move(b);
}
*/
void c::local_delta::compartiments_t::to_stream(ostream& os) const {
	os << size() << ' ';
	for (auto& i:*this) {
		os << i.first << ' ';
		i.second.to_stream(os);
	}
}

c::local_delta::compartiments_t* c::local_delta::compartiments_t::from_stream(istream& is) {
	compartiments_t* r=new compartiments_t();
	size_t n;
	is >> n;
	for (size_t i=0; i<n; ++i) {
		hash_t h;
		is >> h;
		r->emplace(h,move(compartiment_t::from_stream(is)));
	}	
	return r;
}

void c::local_delta::logbook_t::compute_hash(hasher_t& h) const {
    for (auto&i:items) {
        h.write(i);
    }
    for (auto&i:*this) {
        h.write(i);
    }
}

void c::local_delta::compartiment_t::compute_hash(hasher_t& h) const {
	h.write(locking_program);
	logbook.compute_hash(h);
}

void c::local_delta::compartiments_t::compute_hash(hasher_t& h) const {
	for (auto&i:*this) {
		h.write(i.first);
		i.second.compute_hash(h);
	}
}


const hash_t& c::local_delta::get_hash() const {
	if (hash==0) hash=compute_hash();
	return hash;
}

hash_t c::local_delta::compute_hash() const { //include only the elements controlled by majority_consensus
	hasher_t h;
    compartiments.compute_hash(h);
//	h.write(fees);
	hash_t v;
	h.finalize(v);
	return move(v);
}

uint64_t c::delta::merge(blockchain::app::local_delta* other0) {
	local_delta* other=static_cast<local_delta*>(other0);
	m->merge(*other,*other);		

	b::merge(other0);

//cout << "MERGE: other.fees=" << other->fees << endl;
	return 0; //other->fees;
}

void c::delta::end_merge() {
	m->end_merge(g, 0);
//cout << "END MERGE: g.fees=" << g.fees << endl;

//			double m=multiplicity;
//			for (int i=0; i<policies_traits::num_params; ++i) policies[i]/=m;
}


c::delta::delta():m(new blockchain::majority_merger<local_delta>) {
}

c::delta::~delta() {
	delete m;
}

c::db_t::db_t() {
	clear();
}
c::db_t::db_t(db_t&& other) {
	compartiments=other.compartiments;
	other.compartiments=0;
}
c::db_t::~db_t() {
	delete compartiments;
}


