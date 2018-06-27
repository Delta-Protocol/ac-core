#include "wallet.h"
#include "protocol.h"

using namespace us::wallet;
using namespace std;
typedef us::wallet::wallet c;


c::wallet(const string& datapath, const string& backend_host, uint16_t backend_port):datapath(datapath), backend_host(backend_host), backend_port(backend_port){
	if (!load()) {
		cerr << "cannot find wallet file in " << datapath << endl;
		exit(1);
	}
}

c::~wallet() {
//cout << "deleting wallet" << endl;
	save();
}

string c::filename() const {
	auto file=datapath+"/wallet";
    return file;
}


bool c::load() {
	auto file=filename();
	if (!file_exists(file)) return true;
	ifstream f(file);
	while(f.good()) {
		string pkb58;
		f >> pkb58;
		if (pkb58.empty()) continue;
		crypto::ec::keys k(crypto::ec::keys::priv_t::from_b58(pkb58));
		cash::hash_t h=k.pub.compute_hash(); //cash::hash_t::from_b58("2vVN9EUdmZ5ypMe84JrQqwExMRjn");
//			cout << "loaded addr " << h << endl;
		emplace(h,move(k));
	}
	return true;
}

bool c::save() const {
//cout << "saving wallet" << endl;

	if (!need_save) return true;
	auto file=filename();
//cout << "saving wallet! " << file << " " << size() << endl;
	ofstream f(file);
	for (auto&i:*this) {
		f << i.second.priv << ' ';
	}
	need_save=false;
	return true;
}

cash::hash_t c::new_address() {
	crypto::ec::keys k=crypto::ec::keys::generate();
	auto h=k.pub.compute_hash();
	emplace(h,move(k));
	need_save=true;
	return move(h);
}
cash::hash_t c::add_address(const crypto::ec::keys::priv_t& key) {
	crypto::ec::keys k(key);
	auto h=k.pub.compute_hash();
	emplace(h,move(k));
	need_save=true;
	return move(h);
}
const crypto::ec::keys* c::get_keys(const cash::hash_t& address) const {
	auto i=find(address);
	if (i==end()) return 0;
	return &i->second;
}

cash::cash_t c::balance() const {
	cash::cash_t b=0;
	for (auto& i:data) {
		b+=i.second.balance;
	}
	return b;
}

void c::dump_balances(ostream& os) const {
	cash::cash_t b=0;
	os << "[address] [locking_program] [balance]" << endl;
	for (auto& i:data) {
		b+=i.second.balance;
		os << i.first << ' ' << i.second.locking_program << ' ' << i.second.balance << endl;
	}
	os << "total balance: " << b << endl;
}

c::accounts_query_t c::query_accounts(const cash::app::query_accounts_t& addresses) const {
	accounts_query_t ret;
	socket::datagram* d=addresses.get_datagram();
	if (!d) return ret;

	socket::datagram* response_datagram=socket::peer_t::send_recv(backend_host,backend_port,d);
	if (!response_datagram) return move(ret);
	auto r=response_datagram->parse_string();
	delete response_datagram;

	istringstream is(r);
	int code;
	is >> code;
	if (code!=0) {	
		string err;
		is >> err;
		cerr << err << endl;
	}
	else {
		for (auto&i:addresses) {
			cash::app::account_t a=cash::app::account_t::from_stream(is);
			if (a.balance>0) {
				ret.emplace(i,move(a));
			}
		}
		is >> ret.parent_block;
	}
	return move(ret);
}

c::compartiments_query_t c::query_compartiments(const nova::app::query_compartiments_t& addresses) const {
	compartiments_query_t ret;
	socket::datagram* d=addresses.get_datagram();
	if (!d) return ret;

	socket::datagram* response_datagram=socket::peer_t::send_recv(backend_host,backend_port,d);
	if (!response_datagram) return move(ret);
	auto r=response_datagram->parse_string();
//cout << "raw ans: " << r << endl;
	delete response_datagram;

	istringstream is(r);
	int code;
	is >> code;
	if (code!=0) {	
		string err;
		is >> err;
		cerr << err << endl;
	}
	else {
		for (auto&i:addresses) {
			nova::app::compartiment_t a=nova::app::compartiment_t::from_stream(is);
			ret.emplace(i,move(a));
		}
		is >> ret.parent_block;
	}
	return move(ret);
}

void c::refresh() {
	cash::app::query_accounts_t addresses;
	addresses.reserve(size());
	for (auto&i:*this) {
		addresses.emplace_back(i.first);
	}
	data=query_accounts(addresses);
}

c::input_accounts_t c::select_sources( const cash::cash_t& amount) {
	refresh();
	vector<accounts_query_t::const_iterator> v;	
	v.reserve(data.size());
	for (accounts_query_t::const_iterator i=data.begin(); i!=data.end(); ++i) {
		v.emplace_back(i);
	}
	
	sort(v.begin(),v.end(),[](const accounts_query_t::const_iterator&v1, const accounts_query_t::const_iterator&v2) { return v1->second.balance < v2->second.balance; });
	input_accounts_t ans;
	ans.parent_block=data.parent_block;
	cash::cash_t remaining=amount;
	for (auto&i:v) {
		if (i->second.balance<=remaining) {
			ans.emplace_back(input_account_t(i->first,i->second,i->second.balance));
			remaining-=i->second.balance;
		}
		else {
			ans.emplace_back(input_account_t(i->first,i->second,remaining));
			remaining=0;
			break;
		}
		
	}
	return ans;
}

void c::dump(ostream& os) const {
	os << "[private Key] [public key] [address]" << endl;
	int n=0;
	for (auto&i:*this) {
		os << '#' << n++ << ": " << i.second.priv << ' ' << i.second.pub << ' ' << i.first << endl;
	}
}

void c::accounts_query_t::dump(ostream& os) const {
	b::dump(os);
	os << "parent block: " << parent_block << endl;
}
void c::compartiments_query_t::dump(ostream& os) const {
	b::dump(os);
	os << "parent block: " << parent_block << endl;
}



c::input_account_t::input_account_t(const hash_t& address,const b& acc, const cash_t& withdraw_amount):b(acc),address(address),withdraw_amount(withdraw_amount) {
}

void c::input_account_t::dump(ostream& os) const {
	os << "address " << address << ' ';
	b::dump(os);
	os << " withdraw amount: " << withdraw_amount;
}

cash::cash_t c::input_accounts_t::get_withdraw_amount() const {
	cash::cash_t w=0;
	for (auto&i:*this) {
		w+=i.withdraw_amount;
	}
	return move(w);
}

void c::input_accounts_t::dump(ostream& os) const {
	for (auto&i:*this) {
		i.dump(os);
		os << endl;
	}
	os << "parent block: " << parent_block << endl;
	os << "total withdraw: " << get_withdraw_amount() << endl;
}

#include <us/gov/cash/locking_programs/p2pkh.h>
#include <us/gov/nova/locking_programs/single_signature.h>


void c::send(const nova::evidence_load& t) const {
	socket::peer_t cli;
	if (!cli.connect(backend_host,backend_port,true)) {
		cerr << "wallet: unable to connect to " << backend_host << ":" << backend_port << endl;
		exit(1);
	}
	cli.send(t.get_datagram());
}

void c::send(const nova::evidence_track& t) const {
	socket::peer_t cli;
	if (!cli.connect(backend_host,backend_port,true)) {
		cerr << "wallet: unable to connect to " << backend_host << ":" << backend_port << endl;
		exit(1);
	}
	cli.send(t.get_datagram());
}

void c::send(const cash::tx& t) const {
	auto fee=t.check();
	if (fee<=0) {
		cerr << "Individual inputs and fees must be positive." << endl;
		exit(1);
	}
	socket::peer_t cli;
	if (!cli.connect(backend_host,backend_port,true)) {
		cerr << "wallet: unable to connect to " << backend_host << ":" << backend_port << endl;
		exit(1);
	}
	cli.send(t.get_datagram());
}

string c::generate_locking_program_input(const crypto::ec::sigmsg_hasher_t::value_type& msg, const nova::hash_t& compartiment, const nova::hash_t& locking_program) {
	if (likely(locking_program<nova::min_locking_program)) {
		if (unlikely(locking_program==0)) {
			return "";
		}
		else if (locking_program==1) {
cout << "-------" << endl; 
			const crypto::ec::keys* k=get_keys(compartiment);
			if (k==0) return "";
			return nova::single_signature::create_input(msg, k->priv);
		}
	}
	return "";
}

string c::generate_locking_program_input(const crypto::ec::sigmsg_hasher_t::value_type& msg, const cash::tx::sigcodes_t& sigcodes, const cash::hash_t& address, const cash::hash_t& locking_program) {
	if (likely(locking_program<cash::min_locking_program)) {
		if (unlikely(locking_program==0)) {
			return "";
		}
		else if (locking_program==1) {
			const crypto::ec::keys* k=get_keys(address);
			if (k==0) return "";
			return cash::p2pkh::create_input(msg, sigcodes, k->priv);
		}
	}
	return "";
}

string c::generate_locking_program_input(const cash::tx& t, size_t this_index, const cash::tx::sigcodes_t& sigcodes, const cash::hash_t& address, const cash::hash_t& locking_program) {
	if (likely(locking_program<cash::min_locking_program)) {
		if (unlikely(locking_program==0)) {
			return "";
		}
		else if (locking_program==1) {
			const crypto::ec::keys* k=get_keys(address);
			if (k==0) return "";
			return cash::p2pkh::create_input(t,this_index, sigcodes, k->priv);
		}
	}
	return "";
}


pair<string,cash::tx> c::tx_sign(const string& txb58, const cash::tx::sigcode_t& sigcodei, const cash::tx::sigcode_t& sigcodeo) {
	auto sigcodes=cash::tx::combine(sigcodei,sigcodeo);

    pair<string,cash::tx> ret;
	ret.second=cash::tx::from_b58(txb58);

	cash::app::query_accounts_t addresses;
	for (auto&i:ret.second.inputs) {
		addresses.emplace_back(i.address);
	}
	wallet::accounts_query_t bases=query_accounts(addresses);
//	bases.dump(cout);
    string err;
	int n=0;
	for (auto&i:ret.second.inputs) {
		auto b=bases.find(i.address);
		if(b==bases.end()) {
			cerr << "No such address " << i.address << endl;
			exit(1);
		}
		const cash::app::account_t& src=b->second;
		if (!cash::app::unlock(i.address, n,src.locking_program,i.locking_program_input,ret.second)) {
			i.locking_program_input=generate_locking_program_input(ret.second,n,sigcodes,i.address, src.locking_program);
			if (!cash::app::unlock(i.address, n,src.locking_program,i.locking_program_input,ret.second)) {	
				i.locking_program_input="";
				ostringstream os;
                os << "cannot unlock account " << i.address << endl;
                ret.first=os.str(); 
			}
		}	
/*
		else {
			cout << "unlocked! " << i.address << endl;
		}
*/
		++n;
	}
    return move(ret);
}

pair<string,cash::tx> c::tx_make_p2pkh(const tx_make_p2pkh_input& i) { //non-empty first means error
        pair<string,cash::tx> ret;
        cash::tx& t=ret.second;

        auto sigcodes=cash::tx::combine(i.sigcode_inputs, i.sigcode_outputs);

		input_accounts_t input_accounts=select_sources(i.amount+i.fee);
		if (input_accounts.empty()) {
			ret.first="no inputs";
			return move(ret);
		}
		if(input_accounts.get_withdraw_amount()!=i.amount+i.fee) {
			ret.first="Failed amounts check";
            return move(ret);
        }

		t.parent_block=input_accounts.parent_block;
		t.inputs.reserve(input_accounts.size());
		for (auto&i:input_accounts) {
			t.add_input(i.address, i.balance, i.withdraw_amount);
		}
		t.add_output(i.rcpt_addr, i.amount, cash::p2pkh::locking_program_hash);
        auto fee=t.check();
    	if (fee<1) { //TODO
            ostringstream err;
            err << "Failed check. fees are " << fee;
			ret.first=err.str();
			return move(ret);
		}
		if (likely(cash::tx::same_sigmsg_across_inputs(sigcodes))) {
			crypto::ec::sigmsg_hasher_t::value_type h=t.get_hash(0, sigcodes);
			int n=0;
			for (auto&i:t.inputs) {
				i.locking_program_input=generate_locking_program_input(h,sigcodes,i.address, input_accounts[n].locking_program);
				++n;
			}
		}
		else { //sigmsg optimization
			int n=0;
			for (auto&i:t.inputs) {
				i.locking_program_input=generate_locking_program_input(t,n,sigcodes,i.address, input_accounts[n].locking_program);
				++n;
			}
		}

		if (i.sendover) {
			send(t);
//			cout << "sent." << endl;
		}

        return move(ret);
}


void c::tx_make_p2pkh_input::to_stream(ostream& os) const {
	os << rcpt_addr << ' ' << amount << ' ' << fee << ' ' << sigcode_inputs << ' ' << sigcode_outputs << ' ' << (sendover?'1':'0');
}

c::tx_make_p2pkh_input c::tx_make_p2pkh_input::from_stream(istream& is) {
	tx_make_p2pkh_input i;
	is >> i.rcpt_addr;
	is >> i.amount;
	is >> i.fee;
	is >> i.sigcode_inputs;
	is >> i.sigcode_outputs;
	is >> i.sendover;
	return move(i);
}

pair<string,nova::evidence_load> c::nova_move(const nova_move_input& i) {
//cout << "nova move" << endl;

    pair<string,nova::evidence_load> ret;
    nova::evidence_load& t=ret.second;

	//blockchain::diff::hash_t parent_block;

	nova::app::query_compartiments_t compartiments;
	compartiments.emplace_back(i.compartiment);

	auto data=query_compartiments(compartiments);
    if (data.size()!=1) {
			ret.first="Compartiment not found";
			return move(ret);
    }
    t.compartiment=i.compartiment;
	t.parent_block=data.parent_block;
    t.load=i.load;
    t.item=i.item;
    t.locking_program=data.begin()->second.locking_program==0?1:data.begin()->second.locking_program; 

//cout << "parent block " <<     t.parent_block << endl;

	crypto::ec::sigmsg_hasher_t::value_type h=t.get_hash();
	t.locking_program_input=generate_locking_program_input(h,i.compartiment,t.locking_program);
//t.write_pretty(cout);
	if (i.sendover) {
		send(t);
//			cout << "sent." << endl;
	}

    return move(ret);
}

pair<string,nova::evidence_track> c::nova_track(const nova_track_input& i) {
//cout << "nova move" << endl;

    pair<string,nova::evidence_track> ret;
    nova::evidence_track& t=ret.second;

	//blockchain::diff::hash_t parent_block;

	nova::app::query_compartiments_t compartiments;
	compartiments.emplace_back(i.compartiment);

	auto data=query_compartiments(compartiments); //TODO distinguish between notfound and lockingprogram==0
    if (data.size()!=1) {
			ret.first="Compartiment not found";
			return move(ret);
    }
    t.compartiment=i.compartiment;
	t.parent_block=data.parent_block;
    t.data=i.data;
    t.locking_program=data.begin()->second.locking_program==0?1:data.begin()->second.locking_program; 

//cout << "parent block " <<     t.parent_block << endl;

	crypto::ec::sigmsg_hasher_t::value_type h=t.get_hash();
	t.locking_program_input=generate_locking_program_input(h,i.compartiment,t.locking_program);
//t.write_pretty(cout);
	if (i.sendover) {
		send(t);
//			cout << "sent." << endl;
	}

    return move(ret);
}

string c::nova_query(const nova::hash_t& compartiment) {
	nova::app::query_compartiments_t compartiments;
	compartiments.emplace_back(compartiment);
	auto data=query_compartiments(compartiments); //TODO distinguish between notfound and lockingprogram==0
    ostringstream os;
    data.pretty_print(os);
    os << "raw answer: ";

    return os.str();
}

string c::nova_mempool() {
	socket::datagram* d=new socket::datagram(us::gov::protocol::nova_mempool_query);
	socket::datagram* response_datagram=socket::peer_t::send_recv(backend_host,backend_port,d);
	if (!response_datagram) return "";
	auto r=response_datagram->parse_string();
	delete response_datagram;
    return r;
}

void c::nova_move_input::to_stream(ostream& os) const {
	os << compartiment << ' ' << item << ' ' << (load?'1':'0') << ' ' << (sendover?'1':'0');
}

c::nova_move_input c::nova_move_input::from_stream(istream& is) {
	nova_move_input i;
	is >> i.compartiment;
    is >> i.item;
	is >> i.load;
	is >> i.sendover;
	return move(i);
}

void c::nova_track_input::to_stream(ostream& os) const {
	os << compartiment << ' ' << data << ' ' << (sendover?'1':'0');
}

c::nova_track_input c::nova_track_input::from_stream(istream& is) {
	nova_track_input i;
	is >> i.compartiment;
    is >> i.data;
	is >> i.sendover;
	return move(i);
}





