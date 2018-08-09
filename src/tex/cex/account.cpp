#include "account.h"
#include "order.h"
#include "engine.h"
#include <boost/filesystem.hpp>
#include <fstream>

using namespace curex::cex;
using namespace std;
typedef curex::cex::account c;



bool c::exists(const engine&e_, const id& id_) {
	return boost::filesystem::exists(data_file(e_,id_).c_str());
}


c::account(engine&e_, const id& id_): e(e_), _id(id_), _commission_discount(0) {
	if (likely(e.persistence())) {
		home=get_home(e_,id_);
		lock_guard<mutex> lock(_mx);
		_load_data();
		_load_wallets();
	}
}


//mutex conmx;

c::account(engine&e_, const string& password): e(e_), _password(hash(password)), _commission_discount(0) {
	_id=e._env.nid_account.get();
	if (likely(e.persistence())) {
		home=get_home(e,_id);
		lock_guard<mutex> lock(_mx);
		boost::filesystem::create_directories(home);
		_save_data();
		_save_wallets();
	}
}	


std::string c::get_home(const engine&e_, const id& theid) {
	std::ostringstream file;
	file << e_._env.nid_account.home << "/";
	id id_=theid;
	while(id_!=0) {
		id r=id_%100;
		id_/=100;
		file << r << "/";
	}
	file << "account_" << theid;
	return file.str();
}

std::string c::data_file() const {
	std::ostringstream os;
	os << home << "/data";
	return os.str();
}
std::string c::data_file(const engine&e_,const id& id_) {
	std::ostringstream os;
	os << get_home(e_,id_) << "/data";
	return os.str();
}
std::string c::wallets_file() const {
	std::ostringstream os;
	os << home << "/wallets";
	return os.str();
}

void c::_load_data() {
	ifstream is(data_file().c_str());
	is >> _password;
	is >> _commission_discount;
}
void c::_load_wallets() {
	clear();
//cerr << wallets_file().c_str() << endl;
	std::ifstream is(wallets_file().c_str());
	while(!is.eof()) {
		std::string line;
		getline(is,line);
		if (line.empty()) continue;
		int curid=0;
		efp av=0;
		{
		std::istringstream is(line);
		is >> curid;
		is >> av;
		}
		emplace(curid,wallet(av));
	}
}

void c::_save_data() const {
	ofstream os(data_file().c_str());
	os << _password << " " << _commission_discount << endl;
}
void c::_save_wallets() const {
	std::ofstream os(wallets_file().c_str());
	for (auto i:*this) {
		os << i.first << " " << i.second.available << endl;
	}
}

void c::save_wallets() const {
	if (unlikely(!e.persistence())) return;
	lock_guard<mutex> lock(_mx);
	_save_wallets();
}

bool c::_new_wallet(int cid) {
	auto i=B::find(cid);
	if (i!=end()) return false;
	B::emplace(cid,wallet());
	if (likely(e.persistence())) _save_wallets();
	return true;
}
bool c::new_wallet(int cid) {
	lock_guard<mutex> lock(_mx);
	return _new_wallet(cid);
}

bool c::delete_wallet(int cid) {
	lock_guard<mutex> lock(_mx);
	auto i=B::find(cid);
	if (i==end()) return false;
	if (i->second.available>0) return false;
	B::erase(i);
	if (likely(e.persistence())) _save_wallets();
	return true;
}
bool c::delete_all_wallets() {
	lock_guard<mutex> lock(_mx);
	for (iterator i=begin(); i!=end(); ) {
		if (i->second.available>0) { ++i; continue; }
		i=B::erase(i);
	}
	if (likely(e.persistence())) _save_wallets();
	return empty();
}
void c::deposit(int cid, const efp& amount) {
	lock_guard<mutex> lock(_mx);
	auto i=B::find(cid);
	if (i==end()) {
		_new_wallet(cid);
		i=B::find(cid);
	}
	i->second.deposit(amount);
	if (likely(e.persistence())) _save_wallets();
}
efp c::withdraw(int cid, const efp& amount) {
	lock_guard<mutex> lock(_mx);
	auto i=B::find(cid);
	if (i==end()) return 0;
	auto r=i->second.withdraw(amount);
	if (likely(e.persistence())) _save_wallets();
	return r;

}

void c::untrack_order(const id& order_id) {
	_orders.erase(order_id);
}
void c::track_order(order& o) {
	_orders.emplace(o._id,&o);
}

double c::get_wallet(int cid) const {
	lock_guard<mutex> lock(_mx);
	auto i=B::find(cid);
	if (i==end()) return 0;
	return decode_dbl(i->second.available);
}

c::dwallets c::get_wallets() const {
	dwallets ans;
	lock_guard<mutex> lock(_mx);
	for (auto& i:*this) {
		ans.emplace(i.first,decode_dbl(i.second.available));
	}
	return move(ans);
}

void c::list(ostream& os) const {
	lock_guard<mutex> lock(_mx);
	for (auto& i:*this) {
		os << i.first << L" " << curstr(i.first) << L" " << decode(i.second.available) << endl;
	}
}
void c::list_columns(ostream& os) {
	os << L"<currency id> <currency name> <available>" << endl;
}


void c::list_orders(ostream& os) const {
	lock_guard<mutex> lock(_orders._mx);
	for (auto o:_orders) {
		o.second->report(os);
	}
}

void c::tradedata::report_columns(ostream& os) {
	os << "M <trade id> <amount paid> <currency paid> <rate> <rate units> <gross amount received> <currency received> <commission paid> <currency commission>" << endl;
}

id c::tradedata::_next_id{1};
mutex c::tradedata::_mx_next_id;


c::tradedata::tradedata(
id order_id,
efp rate, int weak_curid, int strong_curid,
int paid_curid, efp paid_amount,
int received_curid, efp received_amount,
int commission_curid, efp commission_amount,
bool weak,bool buy,bool lay):

 _order_id(order_id), _rate(rate), _weak_curid(weak_curid), _strong_curid(strong_curid), _paid_curid(paid_curid), _paid_amount(paid_amount), _received_curid(received_curid), _received_amount(received_amount), _commission_curid(commission_curid), _commission_amount(commission_amount),_weak(weak), _buy(buy), _lay(lay) {
	lock_guard<mutex> lock(_mx_next_id);
	_trade_id=_next_id++;
}

void c::tradedata::report(ostream& os) const {
	os << L"M " << _trade_id << L" " << decode(_paid_amount) << L" " << curstr(_paid_curid) << " " << decode(_rate) << " " << curstr(_weak_curid) << "/" << curstr(_strong_curid) << " " << decode(_received_amount) << " " << curstr(_received_curid) << " " << decode(_commission_amount) << " " << curstr(_commission_curid) << endl;
}


c::transaction::transaction() {
	lock_guard<mutex> lock(_mx_next_id);
	_transaction_id=_next_id++;
}



void c::transaction::report_columns(ostream& os) {
	os << "M <transaction id> <B|S> <amount> <currency> <rate> <rate units> <gross amount received> <currency received> <commission paid> <currency commission>" << endl;
}

id c::transaction::_next_id{1};
mutex c::transaction::_mx_next_id;

void c::transaction::report(ostream& os) const {
	if (empty()) return;
	const auto& b=*begin();
	os << L"M " << _transaction_id << L" "
	<< (b._buy?L"B ":L"S ") << 
	decode(b._buy?_received_amount:_paid_amount) << L" " << 
	curstr(b._buy?b._received_curid:b._paid_curid) << " " << 
	decode(_rate) << " " << curstr(b._weak_curid) << "/" << curstr(b._strong_curid) << " " <<
	decode(b._buy?_paid_amount:_received_amount) << " " << 
	curstr(b._buy?b._paid_curid:b._received_curid) << " " << 
	decode(_commission_amount) << " " << curstr(b._commission_curid) << /*(b._lay?L"lay ":L"back ") <<*/ endl;
}


void c::transaction::register_trade(tradedata&& td) {
	emplace_back(move(td));
}


void c::transaction::recalculate() {
	_rate=0;
	_paid_amount=0;
	_received_amount=0;
	_commission_amount=0;
	if (empty()) return;
	for (auto& i:*this) {
		_paid_amount+=i._paid_amount;
		_received_amount+=i._received_amount;
		_commission_amount+=i._commission_amount;
	}
//wcout << "paid " << decode(_paid_amount) << " " << curstr(begin()->_paid_curid) << endl;
		if (begin()->_paid_curid==begin()->_weak_curid) {
			_rate=encode(decode_dbl(_paid_amount)/decode_dbl(_received_amount));
		}
		else {
			_rate=encode(decode_dbl(_received_amount)/decode_dbl(_paid_amount));
		}

/*
	}
	else {
		if (begin()->_weak) {
			_rate=encode(decode_dbl(_paid_amount)/decode_dbl(_received_amount));
		}
		else {
			_rate=encode(decode_dbl(_received_amount)/decode_dbl(_paid_amount));
		}
	}	
*/
/*
	if (begin()->_buy ) {
//wcout << " rate1 " << decode(_rate)  << endl;
	_rate=encode(decode_dbl(_paid_amount)/decode_dbl(_received_amount));
	}
	else {
//wcout << " rate1 " << decode(_rate)  << endl;
	}
*/
}



void c::register_trade(tradedata&& td) {
	transaction tr;
	tr.register_trade(move(td));
	tr.recalculate();
	register_transaction(move(tr));
	
}
void c::register_transaction(transaction&& tr) {
	_transactions.register_transaction(move(tr));
}
void c::transactions::register_transaction(transaction&& tr) {
	lock_guard<mutex> lock(_mx);
	emplace_back(move(tr));
}

void c::list_trades(ostream& os) const {
	_transactions.report(os);
}

void c::transactions::report(ostream& os) const {
	lock_guard<mutex> lock(_mx);
	for (auto i=rbegin(); i!=rend(); ++i) {
		i->report(os);
	}
}

/*
void c::dump(const string& prefix, ostream& os) const {
	if (empty()) {
		os << prefix << L"no wallets" << endl;
		return;
	}
	for (auto& i:*this) {
		os << prefix << L"wallet " << curstr(i.first) << L" " << decode(i.second.available) << endl;
	}
}
*/
void c::set_commission_discount(const double&d) {
	if (d>1 || d<0) return;
	lock_guard<mutex> lock(_mx);
	_commission_discount=d;
}

size_t c::hash(const string& s) const {
	static std::hash<string> hash_fn;
    return hash_fn(s);
}

bool c::check_password(const string& pwd) const {
	size_t h=hash(pwd);
	lock_guard<mutex> lock(_mx);
	return h==_password;
}

/*
void c::orderdata::dump(const string& prefix, ostream& os) const {
	os << prefix << L"acid " << _account_id << L" received_cur " << _received_cur << L" paid_cur " << curstr(_paid_cur) << endl;
	_order->dump(prefix+L"  ",os);
}
*/
c::const_iterator c::find(int id_) const {
	lock_guard<mutex> lock(_mx);
	return B::find(id_);
}
c::iterator c::find(int id_) {
	lock_guard<mutex> lock(_mx);
	return B::find(id_);
}



c::orders::B::const_iterator c::orders::find(const id& id_) const {
	lock_guard<mutex> lock(_mx);
	return _find(id_);
}
c::orders::B::const_iterator c::orders::_find(const id& id_) const {
	return B::find(id_);
}
c::orders::B::iterator c::orders::find(const id& id_) {
	lock_guard<mutex> lock(_mx);
	return _find(id_);
}
c::orders::B::iterator c::orders::_find(const id& id_) {
	return B::find(id_);
}
void c::orders::erase(const id& id_) {
	lock_guard<mutex> lock(_mx);
	B::erase(id_);
}
void c::orders::_erase(const id& id_) {
	B::erase(id_);
}
void c::orders::emplace(const id& id_,order*o) {
	lock_guard<mutex> lock(_mx);
	B::emplace(id_,o);
}
void c::orders::_emplace(const id& id_,order*o) {
	B::emplace(id_,o);
}




