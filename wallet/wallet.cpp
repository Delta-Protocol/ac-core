#include "wallet.h"
#include "protocol.h"
#include <us/gov/input.h>

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
//cout << "loading from " << file << endl;
	if (!us::gov::input::cfg::file_exists(file)) return true;
	ifstream f(file);
	while(f.good()) {
		string pkb58;
		f >> pkb58;
		if (pkb58.empty()) continue;
		auto pk=crypto::ec::keys::priv_t::from_b58(pkb58);
	        if (!gov::crypto::ec::keys::verify(pk)) {
        	        cerr << "The private key " << pkb58 << " is incorrect." << endl;
                	continue;
	        }
		crypto::ec::keys k(pk);
		cash::hash_t h=k.pub.compute_hash(); //cash::hash_t::from_b58("2vVN9EUdmZ5ypMe84JrQqwExMRjn");
//			cout << "loaded addr " << h << endl;
		emplace(h,move(k));
	}
	return true;
}

bool c::save() const {
//ofstream cout("/tmp/xxxxxxx");

//cout << "saving wallet " << filename() <<endl;

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
	save();
	return move(h);
}
cash::hash_t c::add_address(const crypto::ec::keys::priv_t& key) {
	if (!gov::crypto::ec::keys::verify(key)) {
		cerr << "Invalid private key" << endl;
		return 0;
	}
	crypto::ec::keys k(key);
	auto h=k.pub.compute_hash();
	emplace(h,move(k));
	need_save=true;
	save();
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

void c::extended_balance(ostream& os) const {
	cash::cash_t b=0;
	os << "[address] [locking_program] [balance]" << endl;
	for (auto& i:data) {
		b+=i.second.balance;
		os << i.first << ' ' << i.second.locking_program << ' ' << i.second.balance << endl;
	}
	os << "total balance: " << b;
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
void c::refresh() {
	cash::app::query_accounts_t addresses;
	addresses.reserve(size());
	for (auto&i:*this) {
cout << "addr " << i.first << endl;
		addresses.emplace_back(i.first);
	}
cout << "query accounts" << endl;
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




