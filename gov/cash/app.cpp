#include "app.h"
//#include "peer.h"
#include "protocol.h"
#include <thread>
#include <chrono>
#include <gov/crypto/base58.h>
#include <gov/likely.h>

typedef us::gov::cash::app c;
using namespace us::gov;
using namespace us::gov::cash;
using namespace std;

constexpr const char* c::name;
//constexpr spend_code_t no_spend_code;
//min_spend_code

//atomic<uint64_t> tx::next_localuid{0};
constexpr array<const char*,app::policies_traits::num_params> app::policies_traits::paramstr;


namespace std {

  template <>
  struct hash<us::gov::cash::app::local_delta> {
	size_t operator()(const us::gov::cash::app::local_delta&) const;
  };

}


size_t std::hash<us::gov::cash::app::local_delta>::operator() (const us::gov::cash::app::local_delta&g) const {
	return *reinterpret_cast<const size_t*>(&g.get_hash()[0]);
}

/*
string tx::input_t::message_to_sign() const {
	ostringstream os;
	os << amount << ' ' << pkh << ' ' << spend_code << ' ' << ;
					hash_t pkh;
					spend_code_t spend_code;
					string lock_program_input;
					cash_t amount;

	return os.str();
}
*/
/*
bool tx::input_t::verify() const {
return false;
//	cout << "verifying input " << pubkey << " " << amount << endl;
//	return crypto::ec::instance.verify(pubkey,message_to_sign(),signature);
}
*/

bool c::local_delta::accounts_t::add_input(tx& t, const hash_t& addr, const cash_t& amount) {
//	spend_code_t sc=lookup_spend_code(addr);
//	if (sc==0) return false;
	cash_t prev_balance=0; //TODO
	return t.add_input(addr,prev_balance, amount/*,sc*/);
}
bool c::local_delta::accounts_t::add_output(tx& t, const hash_t& addr, const cash_t& amount, const hash_t& locking_program) {
	return t.add_output(addr,amount,locking_program);
}



/*
spend_code_t c::accounts_t::lookup_spend_code(const hash_t& address) const {
	auto i=find(address);
	if (i!=end()) return i->second.spend_code;
	return 0;
}
*/
/*
c::accounts_t::spend_code_t c::accounts_t::p2pkh_t::lookup_spend_code(const hash_t& address) const {
	auto i=find(address);
	if (i!=end()) return i->second.first;
	return 0;
}

c::accounts_t::spend_code_t c::accounts_t::stdtx_t::lookup_spend_code(const hash_t& address) const {
	auto i=find(address);
	if (i!=end()) return i->second.second.first;
	return 0;
}

c::accounts_t::spend_code_t c::accounts_t::p2sh_t::lookup_spend_code(const hash_t& address) const {
	auto i=find(address);
	if (i!=end()) return i->second.second.first;
	return 0;
}
*/
/*
c::accounts_t::spend_code_t c::accounts_t::lookup_spend_code(const hash_t& address) const {
	auto sc=p2pkh.lookup_spend_code(address);
	if (sc!=0) return sc;
	sc=stdtx.lookup_spend_code(address);
	if (sc!=0) return sc;
	sc=p2sh.lookup_spend_code(address);
	return sc;
}
*/

int c::local_delta::app_id() const {
	return app::id();
}
/*
bool blockchain_app::in_service() const {
	return true;
}
*/
/*
void app::on_begin_cycle() {
		cout << "app: cash: on_begin_cycle" << endl;
}
*/
/*
unordered_map<const local_deltas*,uint64_t> blockchain_app::to_fees(const unordered_map<const local_deltas*,double>& shares,uint64_t total_fees) {
	unordered_map<const local_deltas*,uint64_t> ans;
	if (total_fees==0) return move(ans);
	double tot_shares=0;
	for (auto& i:shares) {
		cout << "share: " << i.second << endl;
		tot_shares+=i.second;
	}
cout << "to_fees: shares.size " << shares.size() << " total_fees " << total_fees << " tot_shares: " << tot_shares << endl;
	for (auto& i:shares) {
		double f=total_fees*(i.second/tot_shares);
		ans.emplace(i.first,(uint64_t)f);
	}


	uint64_t sum_fees=0;
	for (auto& i:ans) sum_fees+=i.second;

	long long diff=sum_fees-total_fees;
cout << "diff0 " << diff << endl;
	while(true) { //normal case, since tx are repeated more than 1 in the block sum_fees should be greater then total_fees available
		cout << "loop " << endl;
		if (diff<=0) break;
		long long percapita=diff/ans.size();
		cout << "percapita " << percapita << endl;
		if (percapita>0) {
			for(auto& i:ans) i.second-=percapita;
		}
		else {
			auto i=ans.begin();
			while(diff>0) { --i->second; --diff; ++i; }
		}
	}
	assert(diff==0);
	return move(ans);
}
*/
/*
void local_delta::merge(blockchain::local_delta* g) {
	delete g;

}
*/
/*
tx* blockchain_app::pay_to_net(const blockchain::block& b) {
return 0;
/ *
//cout << "pay_to_net A" << endl;
	if (b.empty()) return 0;
	tx* paynet=new tx();
	unordered_set<const tx*> seentx;
	unordered_map<const local_deltas*,double> shares;
	unordered_map<const local_deltas*,string> pubkeys;
	uint64_t total_fees=0;
	for (auto& m:b) {
		pubkeys.emplace(m.second,m.first);
		double miner_fees=0;
		for (auto&bg:*m.second) {
			const app_gut* g=dynamic_cast<app_gut*>(bg.second); // TODO: find it directly with find, not scan, now it is indexed
			if (!g) continue; //not this app
			for (auto& t:*g) {
				auto f=t->fee();
				miner_fees+=f; //increase his share even though th tx has been included by another miner as well
				if (seentx.find(t)!=seentx.end()) continue;
				total_fees+=f; //real total fees, 1 tx - 1 fee
				//paynet->pay(m->addr,t->fee());
				seentx.insert(t);
				//any=true;
			}
		}
		shares.emplace(m.second,miner_fees);
	}
//cout << "pay_to_net B" << endl;
	unordered_map<const local_deltas*,uint64_t> fees=to_fees(shares,total_fees);
	//b.purge(); //delete miners not adding value, no puedo es const, puede haber otras apps que les sirva el gut
	uint64_t coinbase=8e8;
	uint64_t coinbase_per_miner;
	if (fees.empty()) { //distribute coinbase among all who submitted a gut
		coinbase_per_miner=coinbase/b.size();
		for (auto& m:b) {
			paynet->pay(m.first,coinbase_per_miner);
		}
	}
	else {
		coinbase_per_miner=coinbase/fees.size();
		for (auto& m:fees) {
			paynet->pay(pubkeys.find(m.first)->second,coinbase_per_miner+m.second);
		}
	}
//cout << "pay_to_net C" << endl;
	return paynet;
* /
}
*/
/*
blockchain::app_gut* blockchain_app::create_closure_gut(const blockchain::block& b) {
//	cout << "app: cash: create_closure_gut" << endl;
	auto ptn=pay_to_net(b);
	cout << "pay_to_net is " << ptn << endl;

	if (ptn==0) return 0;
	auto* gut=new app_gut();
	gut->push_back(ptn);
	//if (b.checkpoint) {
	//	gut->add_checkpoint();
	//}
//cout << "return gut" << endl;
	return gut;
}
*/

///----------------------------------------------

c::app() {
	pool=new c::local_delta();
	policies_local=policies;
//	verif_thread=new thread(&app::verification_daemon,this);
}

c::~app() { 
	delete pool;
//	verif_thread->join();
//	delete verif_thread;
}
/*
void c::on_head_ready() { //can start verification
	buffer.set_unverified();
}
*/
/*
c::validation_result c::validate(const tx& tr) const { //
        return valid;
}
*/
/*
void c::incoming_transaction_from_node(peer_t *src, datagram*d) {
    auto t=tx::parse_from_node(*d);
    if (validate(*t)==valid) {
		//relay rules:
		//objective: 1 transaction must appear in only a few guts in the block, if every tx appears in every gut of each miner the block would have size O(N*T) where N is number of miners and T number of incoming transactions for this height.
		//		we want block size O(N), each miner shall provide only a small % of transactions.
		//For doing so, propagation of each tx shall be limited.
		//The following rule encourages a miner to both relay the tx and stop relaying it after reached a maximum number of hops is reached.
		//merchants start a transaction using only 3 random nodes, no more, no less, otherwise the tx might not progress to the blockchain.
		//at this point the tx enters the ecosystem, 3 different propagation threads has been initiated, nodes will assign hop number 0.
		//propagation threads consist on 4 hops.
		//according to the rule a total of 12 nodes see the same tx.
		//block validation will check how many guts from different miners contain the same transaction
		//the tx will be discarded if this number is below 6 (some miner refused to relay, trying to maximize chances of getting its fee)
		//the tx will be discarded if this number is above 12 (some miner relayed in excess, increasing system congestion)
		//Particularity: on reception the node will check if it has seen the tx before.
		//if seen before, it relays it without incrementing the hop number, so every propagation thread can provide 4 different guts containing the tx.
		//in total a maximum of 12 guts is expected when making the closure block
		//this miner has 1/16 chances of getting paid the fee for this tx.
		bool first_time=buffer.add(t);
		uint8_t hop=tx::get_hop(*d);
		if (hop<3) {
		//					if (first_time) hop=tx::increment_hop(*d);
			parent->relay(1,src,d); //d is consumed
		}
	}
	else {
	    delete t;
	    delete d;
	    return;
	}
}
*/
/*
void c::incoming_transaction_from_merchant(peer_t *src, datagram*d) {
	auto t=tx::parse_from_merchant(*d);
	delete d;
	d=t->create_fromnode_datagram();
	return incoming_transaction_from_node(src,d);
}
*/
void c::add_policies() {
	lock_guard<mutex> lock(mx_pool);
	lock_guard<mutex> lock2(mx_policies);
	for (int i=0; i<policies_traits::num_params; ++i)
		(*pool)[i] = policies_local[i];
}


blockchain::app::local_delta* c::create_local_delta() {
	//{
	//lock_guard<mutex> lock(buffer.mx);
	cout << "app: cash: create_local_delta " << endl; // << buffer.size() << " verified/unverified txs in mempool. ";
	//}
	add_policies();
	lock_guard<mutex> lock(mx_pool);
	auto full=pool;
	pool=new c::local_delta();
	//full->to_stream(cout);
	/*
	if (full->empty()) {
		delete full;
		return 0;
	}
	*/

	return full; //send collected transactions to the network
}

bool c::process_query(peer_t *c, datagram*d) {
cout << "SGT-01-RECEIVED QUERY" << endl; //settlement go throught
	switch(d->service) {
		case protocol::cash_query: {
			cash_query(c,d);
			return true;
		} break;
	}
	return false;
}

bool c::process_evidence(peer_t *c, datagram*d) {
cout << "SGT-01-RECEIVED EVIDENCE" << endl; //settlement go throught
	switch(d->service) {
		case protocol::cash_tx: {
			string payload=d->parse_string();
cout << "SGT-01-CASH TX " << payload << endl; 
			delete d;
			tx t=tx::from_b58(payload);
t.write_pretty(cout);
			process(t); 
			return true;
		} break;
	}
	return false;
}

c::query_accounts_t c::query_accounts_t::from_string(const string&s) {
	int n;
	istringstream is(s);
	is >> n;
	query_accounts_t r;
	r.reserve(n);
	for (int i=0; i<n; ++i) {
		hash_t v;
		is >> v;
		r.emplace_back(move(v));
	}
	return move(r);	
}

c::query_accounts_t c::query_accounts_t::from_datagram(datagram*d) {
	string query=d->parse_string();
	delete d;
	return from_string(query);
}

void c::cash_query(peer_t *c, datagram*d) {
cout << "CASH_QUERY" << endl;
	query_accounts_t q=query_accounts_t::from_datagram(d);

	ostringstream os;
	os << "0 "; //ret code ok
	{
	lock_guard<mutex> lock(db.mx);
	for (auto&addr:q) {
		auto a=db.accounts->find(addr);
		if (likely(a!=db.accounts->end())) {
			a->second.to_stream(os);
		}
		else {
			account_t a;
			a.locking_program=0;
			a.balance=0;
			a.to_stream(os);
		}
		
	}
	}

	os << ' ' << last_block_imported;

cout << "CASH_RESPONSE " << os.str() << endl;

	c->send(protocol::cash_response,os.str());	
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
void c::accounts_t::p2pkh_t::dump(ostream& os) const {
	cout << size() << " p2pkh accounts:" << endl;
	for (auto& i:*this) {
		cout << ' ' << i.first << ' ' << i.second.second << " (spend-code " << i.second.first << ")" << endl;
	}
}
void c::accounts_t::stdtx_t::dump(ostream& os) const {
	cout << size() << " stdus accounts:" << endl;
	for (auto& i:*this) {
		cout << ' ' << i.first << ' ' << i.second.first << ' ' << i.second.second.second << " (spend-code " << i.second.second.first << ")" << endl;
	}
}
void c::accounts_t::p2sh_t::dump(ostream& os) const {
	cout << size() << " p2sh accounts:" << endl;
	for (auto& i:*this) {
		cout << ' ' << i.first << ' ' << i.second.first << ' ' << i.second.second.second << " (spend-code " << i.second.second.first << ")" << endl;
	}
}
*/

void c::local_delta::account_t::dump(ostream& os) const {
 os << "locking_program " << locking_program << "; balance " << balance; // << "; spend code " << spend_code;
}

void c::local_delta::accounts_t::dump(ostream& os) const {
	cout << size() << " p2sh accounts:" << endl;
	for (auto& i:*this) {
		cout << ' ' << i.first << ' ';
		i.second.dump(os);
		os << endl;
	}
/*
	p2pkh.dump(os);
	stdtx.dump(os);
	p2sh.dump(os);
*/
}

void c::db_t::dump(ostream& os) const {
	cout << "cash app db dump" << endl;
	lock_guard<mutex> lock(mx);
	accounts->dump(os);
	cout << "supply_left " << supply_left << " block_reward " << block_reward << endl;
}

void c::db_t::to_stream(ostream&os) const {
	accounts->to_stream(os);
	os << ' ';
	os << supply_left << ' ' << block_reward;
}

c::db_t c::db_t::from_stream(istream& is) {
	db_t db;
	db.accounts=accounts_t::from_stream(is);
	is >> db.supply_left;
	is >> db.block_reward;
	return move(db);
}

/*
#include <limits>
spend_code_t c::accounts_t::new_spend_code(const spend_code_t& prev) const {
	if (unlikely(prev==numeric_limits<spend_code_t>::max())) return 0;
	return prev+1;
}
*/
/*
c::spend_code_t c::accounts_t::new_spend_code(unsigned int seed) const {
	default_random_engine generator(seed);
	uniform_int_distribution<spend_code_t> distribution(0,numeric_limits<spend_code_t>::max());
	return distribution(generator);
}
*/
bool c::local_delta::accounts_t::pay(const hash_t& address, const cash_t& amount) { //caller has to get the lock
	if (amount<=0) return false;
	auto i=find(address);
	if (i==end()) {
		emplace(make_pair(address,account_t{1,amount}));
	}
	else {
		//same spend_code
		i->second.balance+=amount;
	}
	return true;
}



//void c::accounts_t::rollback(undo_t& undo) {
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




bool c::local_delta::accounts_t::withdraw(const hash_t& address, const cash_t& amount/*, undo_t& undo*/) { //caller has to get the lock
	auto i=find(address);
	if (i==end()) {
		return false;
	}
/*
	else if (i->second.spend_code!=sc) {
		return false;
	}
*/
	else if (i->second.balance<amount) {
		return false;
	}

		//locate the output;
//	undo.add_step(i->first,i->second.spend_code,i->second.balance);
	
	i->second.balance-=amount;
	if (unlikely(i->second.balance==0)) {
		erase(i);
//		i->second.spend_code=new_spend_code(seed);
	}
//	else {
//		undo.mark_delete();
//	}
	return true;
}



bool c::db_t::add_(const hash_t& k, const cash_t& amount) { //caller has to get the lock
	return accounts->pay(k,amount);
}


bool c::db_t::withdraw_(const hash_t& k, const cash_t& amount/*, accounts_t::undo_t& undo*/) { //caller has to get the lock
	return accounts->withdraw(k,amount/*,undo*/);
}

/*
bool c::db_t::move(const tx::end_t& srcs, const tx::end_t& dsts) {
	vector<pair<accounts_::iterator,uint64_t>> inputs;
	vector<pair<accounts_::iterator,uint64_t>> outputs;
	uint64_t sum_inputs=0;
	uint64_t sum_outputs=0;
	lock_guard<mutex> lock(mx);
	for (auto& src:srcs) {
		auto s=accounts.find(src.first);
		if (s==accounts.end()) return false;
		if (s->second<src.second) return false;
		inputs.push_back(make_pair(s,src.second));
		sum_inputs+=src.second;
	}
	vector<pair<string,uint64_t>> new_outputs;
	for (auto& dst:dsts) {
		auto s=accounts.find(dst.first);
		if (s==accounts.end()) {
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
		if (i.first->second==0) accounts.erase(i.first);
	}
	for (auto& i: outputs) {
			i.first->second+=i.second;
	}
	for (auto& i: new_outputs) {
		accounts.emplace(i.first,i.second);
	}
	return true;
}

void c::clear_db() {
	db.clear();
}

bool c::db_t::move(const string& src, const string& dst, uint64_t amount) {
	lock_guard<mutex> lock(mx);
	auto s=accounts.find(src);
	if (s==accounts.end()) return false;
	if (s->second<amount) return false;
	if (s->second==amount) 
		accounts.erase(s);
	else 
		s->second-=amount;
	auto d=accounts.find(dst);
	if (d==accounts.end()) {
		accounts.emplace(dst,amount);
	}
	else {
		d->second+=amount;
	}
	return true;
}

void c::db_t::clear() {
	lock_guard<mutex> lock(mx);
	accounts.clear();
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

void c::import(const blockchain::app::delta& gg, const blockchain::pow_t& w) {
cout << "cash: importING appgut2 MULTIPLICITY " << gg.multiplicity << endl;
	const delta& g=static_cast<const delta&>(gg);
	{
	lock_guard<mutex> lock(mx_policies);
	for (int i=0; i<policies_traits::num_params; ++i) policies[i]=g[i];
	}

//patch db.

	{
	const c::local_delta::accounts_t& a=g.g.accounts;
	lock_guard<mutex> lock(db.mx);
	for (auto& i:a) {
//cout << "AAAAAAAAAAAAAAAAA- " << i.first << endl;

		auto d=db.accounts->find(i.first);
		if (d==db.accounts->end()) {
			if (likely(i.second.balance!=0)) {
//cout << "AAAAAAAAAAAAAAAAAA " << i.second.locking_program << endl;
				db.accounts->emplace(i.first,account_t(i.second.locking_program,i.second.balance));
			}
		}
		else {
			if (i.second.balance==0) {
//cout << "AAAAAAAAAAAAAAAAA3" << endl;
				db.accounts->erase(d);
			}
			else {
//cout << "AAAAAAAAAAAAAAAAA2 " << i.second.locking_program << endl;
				d->second.locking_program=i.second.locking_program;
				d->second.balance=i.second.balance;
			}
		}
	}

	}

//spread fees

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
}

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


#include "locking_programs/p2pkh.h"

bool c::unlock(const hash_t& address, const size_t& this_index, const hash_t& locking_program, const string& locking_program_input, const tx& t) {
	if (likely(locking_program<min_locking_program)) {
		if (unlikely(locking_program==0)) {
			return true;
		}
		else if (locking_program==p2pkh::locking_program_hash) {
			return p2pkh::check_input(address, t,this_index, locking_program_input);
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

bool c::account_state(const local_delta::batch_t& batch, const hash_t& address, account_t& acc) const {
cout << "SGT-03-Acc state " << " address " << address << endl; 
		auto  b=batch.find(address);
		if (likely(b==batch.end())) { //this input has already been consumed at least once
cout << "SGT-03-Acc state " << " not in batch " << endl; 
			auto p=pool->accounts.find(address);
			if (likely(p==pool->accounts.end())) { //ref in db
cout << "SGT-03-Acc state " << " not in pool " << endl; 

				lock_guard<mutex> lock(db.mx);
				auto o=db.accounts->find(address);
				if (o==db.accounts->end()) {
cout << "SGT-03-Acc state " << " not in db " << endl; 
					return false;
				}
				acc.locking_program=o->second.locking_program;
				acc.balance=o->second.balance;
			}
			else { //ref in pool
cout << "SGT-03-Acc state " << " in pool " << endl; 
				acc.locking_program=p->second.locking_program;
				acc.balance=p->second.balance;
			}

		}
		else { //ref in batch
cout << "SGT-03-Acc state " << " in batch " << endl; 
/*
			if (b->second.spend_code!=i.spend_code) { //if spend_code does not match the expected one, assume this tx has been already processed
				return false;
			}
*/
			acc.locking_program=b->second.locking_program;
			acc.balance=b->second.balance;
		}
		return true;
cout << "SGT-03-Acc state " << " locking_program  " << acc.locking_program << endl; 
cout << "SGT-03-Acc state " << " balance  " << acc.balance << endl; 
}

bool c::process(const tx& t) {
	cout << "cash: Processing transaction " << endl;

	if (t.parent_block!=last_block_imported) {
		cout << "SGT-02-tx.rejected - base mismatch - " << t.parent_block << " != base:" << last_block_imported << endl; 
		return false;
	}

	cash_t min_fee;
	{
	lock_guard<mutex> lock(mx_policies);
	min_fee=policies[policies_traits::minimum_fee];
	}

	if (min_fee<0) return false;
cout << "SGT-02-tx.check (I>O,fees) " << endl; 

	auto fee=t.check();
	if (fee<min_fee) return false;

	//tx verification

//	auto seed=parent->get_seed();

	local_delta::batch_t batch;

//	accounts_t::undo_t undo;
//	undo.reserve(t.inputs.size());

//	hash_t lp;
//	cash_t balance;
	account_t state; 

cout << "SGT-02-tx.I size " << t.inputs.size() << endl; 
	for (size_t j=0; j<t.inputs.size(); ++j) {
		auto& i=t.inputs[j];
//
		if (!account_state(batch,i.address,state)) {
cout << "SGT-02-tx.Input #" << j << " account_state returned false.DENIED " << endl; 
			return false;
		}

cout << "SGT-02-tx.Input #" << j << " UNLOCK.." << endl; 
		if (!unlock(i.address, j,state.locking_program, i.locking_program_input, t)) {
cout << "SGT-02-tx.Input #" << j << " UNABLE TO UNLOCK. denied." << endl; 
			return false;
		}

		state.balance-=i.amount;
cout << "SGT-02-tx.Input #" << j << " BATCH ADD.." << endl; 
		batch.add(i.address,state);
cout << "SGT-02-tx.Input #" << j << " final balance " << state.balance << endl; 

/*
		if (!db.withdraw_(seed, i.spend_code, i.address, i.amount, undo)) {
			db.accounts.rollback(undo);
			return false;
		}
*/
	}

cout << "SGT-02-tx.Output size " << t.outputs.size() << endl; 
	for (size_t j=0; j<t.outputs.size(); ++j) {
		auto& i=t.outputs[j];
cout << "SGT-02-tx.Output #" << j << " addr " << i.address << endl; 
		if (account_state(batch,i.address,state)) { 
			if (unlikely(state.balance!=0 && state.locking_program!=i.locking_program)) { //locking program can only be replaced when balance is 0
cout << "SGT-02-tx.Output #" << j << " locking program can only be replaced when balance is 0. DENIED." << endl; 
				return false;
			}
			state.balance+=i.amount;
		}
		else { //new output
cout << "SGT-02-tx.Output #" << j << " NEW OUTPUT " << endl; 
			state.locking_program=i.locking_program;
			state.balance=i.amount;
		}


		batch.add(i.address,state);
cout << "SGT-02-tx.Output #" << j << " added to batch." << endl; 
	}
	pool->fees+=fee;

cout << "SGT-02-tx ADD BATCH TO POOL; pool->fees=" << pool->fees << " fee=" << fee << endl;
	pool->accounts.add(batch);
cout << "SGT-02-tx OK" << endl; 
	return true;
}

void c::local_delta::to_stream(ostream& os) const {
	os << accounts.size() << " ";
	for (auto& i:accounts) {
		os << i.first << ' '; // << /*i.second.spend_code << ' ' <<*/ i.second.balance << ' ';
		i.second.to_stream(os);
	}
cout << "appgut WRITE fees: " << fees << endl;
	os << fees << ' ';
	b::to_stream(os);
}

void c::local_delta::from_stream(istream& is) {
	int n;
	is >> n;
	for (int i=0; i<n; ++i) {
		hash_t pubk;
		is >> pubk;

		batch_item bi=batch_item::from_stream(is);
		accounts.emplace(move(pubk),move(bi));
	}
	is >> fees;
cout << "appgut READ fees: " << fees << endl;
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
void us::gov::cash::tx::to_stream(ostream& os) const {
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

void us::gov::cash::tx::from_stream(istream& is, end_t& dest) {
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

tx* us::gov::cash::tx::from_stream(istream& is) {
	tx*t=new tx();
	from_stream(is,t->inputs);
	from_stream(is,t->outputs);
	return t;
}

uint64_t us::gov::cash::tx::fee() const {
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
		os << "cash shell. type h for help." << endl;
	}
	else if (cmd=="h" || cmd=="help") {
		os << "Cash shell." << endl;
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
    	lock_guard<mutex> lock(mx_pool);
        pool->accounts.dump(os);
        os << "fees: " << pool->fees << endl;
        
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
    delete accounts;
	accounts=new accounts_t();
    supply_left=2100000000000000; //21.000.000e8  21e14
    block_reward=500000000; //5e8
}

void c::dbhash(hasher_t&) const {
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

c::local_delta::account_t::account_t() {
}

c::local_delta::account_t::account_t(const hash_t& locking_program, const cash_t& balance): locking_program(locking_program), balance(balance) {
}

void c::local_delta::account_t::to_stream(ostream& os) const {
	os << locking_program << ' ' << balance << ' ';
}

c::local_delta::account_t c::local_delta::account_t::from_stream(istream& is) {
	account_t i;
	is >> i.locking_program;
	is >> i.balance;
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

void c::local_delta::accounts_t::add(const batch_t& batch) {
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

cash_t c::local_delta::accounts_t::get_balance() const {
	cash_t b=0;
	for (auto&i:*this) {
		b+=i.second.balance;
	}
	return move(b);
}

void c::local_delta::accounts_t::to_stream(ostream& os) const {
	os << size() << ' ';
	for (auto& i:*this) {
		os << i.first << ' ';
		i.second.to_stream(os);
	}
}
c::local_delta::accounts_t* c::local_delta::accounts_t::from_stream(istream& is) {
	accounts_t* r=new accounts_t();
	size_t n;
	is >> n;
	for (size_t i=0; i<n; ++i) {
		hash_t h;
		is >> h;
		r->emplace(h,move(account_t::from_stream(is)));
	}	
	return r;
}

const hash_t& c::local_delta::get_hash() const {
	if (hash==0) hash=compute_hash();
	return hash;
}

hash_t c::local_delta::compute_hash() const { //include only the elements controlled by majority_consensus
	hasher_t h;
	for (auto&i:accounts) {
		h.write(i.first);
		h.write(i.second.balance);
		h.write(i.second.locking_program);
	}
	h.write(fees);
	hash_t v;
	h.finalize(v);
	return move(v);
}

uint64_t c::delta::merge(blockchain::app::local_delta* other0) {
	local_delta* other=static_cast<local_delta*>(other0);
	m->merge(*other,*other);		

	b::merge(other0);

cout << "MERGE: other.fees=" << other->fees << endl;
	return 0; //other->fees;
}

void c::delta::end_merge() {
	m->end_merge(g, 0);
cout << "END MERGE: g.fees=" << g.fees << endl;

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
c::db_t::db_t(db_t&& other):supply_left(other.supply_left), block_reward(other.block_reward) {
	accounts=other.accounts;
	other.accounts=0;
}
c::db_t::~db_t() {
	delete accounts;
}



