#include "wallet.h"
#include "protocol.h"

using namespace usgov;
using namespace std;
typedef usgov::wallet c;


c::wallet(const string& datapath):datapath(datapath) {
	if (!load()) {
		cerr << "cannot find wallet file in " << datapath << endl;
		exit(1);
	}
}

c::~wallet() {
	save();
}

bool c::load() {
	auto file=datapath+"/wallet";
	if (!file_exists(file)) return true;
	string filename=datapath+"/wallet";
	ifstream f(filename);
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
	if (!need_save) return true;
	string filename=datapath+"/wallet";
	ofstream f(filename);
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

c::accounts_query_t c::get_accounts(const cash::app::query_accounts_t& addresses) {
	accounts_query_t ret;
	socket::datagram* d=addresses.get_datagram();
	if (!d) return ret;

	string addr="92.51.240.61"; //"127.0.0.1";
	uint16_t port=16672;
	socket::datagram* response_datagram=socket::peer_t::send_recv(addr,port,d);
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
		addresses.emplace_back(i.first);
	}
	data=get_accounts(addresses);
}

c::input_accounts_t c::select_sources(const cash::cash_t& amount) {
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

