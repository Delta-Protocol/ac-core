#include "app.h"
#include <gov/peer.h>
#include <thread>
#include <chrono>
#include <gov/likely.h>

typedef usgov::loan::app c;
using namespace usgov;
using namespace usgov::loan;
using namespace std;

constexpr const char* c::name;

constexpr array<const char*,policies_traits::num_params> policies_traits::paramstr;


int local_delta::app_id() const {
	return app::id();
}
/*
bool blockchain_app::in_service() const {
	return true;
}
*/
/*
void app::on_begin_cycle() {
		cout << "app: loan: on_begin_cycle" << endl;
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
void app_gut::merge(blockchain::app_gut* g) {
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
	pool=new loan::local_delta();
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
	cout << "app: loan: create_local_delta " << endl; // << buffer.size() << " verified/unverified txs in mempool. ";
	//}
	add_policies();
	lock_guard<mutex> lock(mx_pool);
	auto full=pool;
	pool=new loan::local_delta();
	//full->to_stream(cout);
	/*
	if (full->empty()) {
		delete full;
		return 0;
	}
	*/

	return full; //send collected transactions to the network
}

/*
bool c::process_work(peer_t *c, datagram*d) {
	return false;
/ *
	switch(d->service) {
		case svc_tx_merchant: incoming_transaction_from_merchant(c,d); break;
		case svc_tx_node: incoming_transaction_from_node(c,d); break;
		default: return false;
	}
	return true;
* /
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
/*
void c::run() {
	while(!program::_this.terminated) {
//		cout << "APP: auth: doing something" << endl;
		db.dump(cout);

		thread_::_this.sleep_for(chrono::seconds(10));
		//parent->peerd.

	}
}
*/
void c::dbhash(hasher_t&) const {
}

void c::db_t::dump(ostream& os) const {
	cout << "loan app db dump" << endl;
/*
	lock_guard<mutex> lock(mx);
	cout << accounts.size() << " accounts:" << endl;
	for (auto& i:accounts) {
		cout << "  " << i.first << " " << i.second << endl;
	}
	cout << "supply_left " << supply_left << " block_reward " << block_reward << endl;
*/
}

//void c::db_t::add_(const hash_t& k, cash_t& amount) { //caller has to get the lock
/*
		auto i=accounts.find(k);
		if (i==accounts.end()) {
			accounts.emplace(k,amount);
		}
		else {
			i->second+=amount;
		}
*/
//}

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

void c::import(const blockchain::app::delta& gg, const blockchain::pow_t& w) {
cout << "loan: importING appgut2 MULTIPLICITY " << gg.multiplicity << endl;
/*
	const delta& g=static_cast<const delta&>(gg);
	{
	lock_guard<mutex> lock(mx_policies);
	for (int i=0; i<policies_traits::num_params; ++i) policies[i]=g[i];
	}


	if (likely(!w.empty())) {
	///spread newcash+fees weighted by work
		lock_guard<mutex> lock(db.mx);
		cash_t newcash=db.get_newcash();
		cash_t total_cash_int=newcash+g.fees;
cout << "total_cash_int " << total_cash_int << endl;
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
			auto seed=parent->get_seed();
			default_random_engine generator(seed);
			uniform_int_distribution<size_t> distribution(0,w.size()-1);
			auto i=w.begin();
			advance(i,distribution(generator));
			cout << "lucky guy " << i->first << " got remainder " << remainder << endl;
			db.add_(i->first,remainder);
		}
	}
*/
}
/*
bool c::process(const tx& t) {
	cout << "cash: Processing transaction " << endl;
	cash_t min_fee;
	{
	lock_guard<mutex> lock(mx_policies);
	min_fee=policies[policies_traits::minimum_fee];
	}

	if (!t.verify(min_fee)) {
		cout << "cash: tx verification failed" << endl;
		return false;
	}


	return true;
}
*/
void usgov::loan::local_delta::to_stream(ostream& os) const {
/*
	os << accounts.size() << " ";
	for (auto& i:accounts) {
		os << i.first << " " << i.second << " ";
	}
	os << fees << " ";
*/
	b::to_stream(os);
}

void usgov::loan::local_delta::from_stream(istream& is) {
/*
	int n;
	is >> n;
	for (int i=0; i<n; ++i) {
		string pubk;
		cash_t p;
		is >> pubk;
		is >> p;
		accounts.emplace(move(pubk),p);
	}
	is >> fees;
*/
	b::from_stream(is);
}

void usgov::loan::delta::to_stream(ostream& os) const {
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
	b::b1::to_stream(os);

}

delta* usgov::loan::delta::from_stream(istream& is) {
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
		os << "loan shell. type h for help." << endl;
	}
	else if (cmd=="h" || cmd=="help") {
		os << "loan shell." << endl;
		os << "h|help              Shows this help." << endl;
		os << "p|policies [id vote]          ." << endl;
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
	else if (cmd=="exit") {
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


