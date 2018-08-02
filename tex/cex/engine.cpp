#include "engine.h"
#include "order.h"
#include "account.h"
#include <algorithm>
#include <fstream>

//#define DEBUG
//#define ELOG

using namespace curex::cex;
using namespace std;

typedef curex::cex::engine c;

#ifdef DEBUG
	#define debug(s) s;
	#ifdef ELOG
		std::ofstream elog("/tmp/elog",ios::app);
	#else
		std::ofstream elog(0);
	#endif
	curex::cex::string strcast(const std::string& o) {
		curex::cex::string ans;
		ans.assign(o.begin(), o.end());
		return ans;
	}
#else
	#define debug(s) ;
#endif

c::engine(std::string home, ostream &os): _env(home), system_acc_pwd(L"11111111") {
debug(os << L"----------------------------");
debug(os << L"engine home:" << strcast(home) << endl);
debug(elog << "engine home:" << home << endl);
	_system_account=get_account(1);

	if (_system_account==0) {
		id id_=new_account(system_acc_pwd,os);
		assert(id_==1);
		_system_account=get_account(1);
	}

	create_markets();
	add_sample_rulers(os);
	track_orders();
	
}
c::~engine() {
	for (auto i:_markets) {
		delete i.second;
	}
	for (auto i:_accounts) {
		delete i.second;
	}

}

void c::track_orders() {
	{
	market* m=&get_market(eur::id,gbp::id);
	track_orders(*m);
	}
	{
	market* m=&get_market(usd::id,eur::id);
	track_orders(*m);
	}
	{
	market* m=&get_market(aud::id,eur::id);
	track_orders(*m);
	}	
	{
	market* m=&get_market(usd::id,gbp::id);
	track_orders(*m);
	}	
	{
	market* m=&get_market(aud::id,gbp::id);
	track_orders(*m);
	}	
	{
	market* m=&get_market(aud::id,usd::id);
	track_orders(*m);
	}	

}

void c::create_markets() {
	{
	market* m=new market(*this,eur::id,gbp::id);
	_markets.emplace(m->get_key(),m);
	track_orders(*m);
	}

	{
	market* m=new market(*this,usd::id,eur::id);
	_markets.emplace(m->get_key(),m);
	}

	{
	market* m=new market(*this,aud::id,eur::id);
	_markets.emplace(m->get_key(),m);
	}

	{
	market* m=new market(*this,usd::id,gbp::id);
	_markets.emplace(m->get_key(),m);
	}

	{
	market* m=new market(*this,aud::id,gbp::id);
	_markets.emplace(m->get_key(),m);
	}

	{
	market* m=new market(*this,aud::id,usd::id);
	_markets.emplace(m->get_key(),m);
	}
}

/*
void c::set_ref_rate(int cid1, int cid2, const double& ref) {
	market::key mkey(cid1,cid2);
        auto m=_markets.find(mkey);
        if (m==_markets.end()) {
                return;
        }
        m->second->set_ref_rate(encode(ref));

}
*/
void c::set_ruler(int cid1, int cid2, const double& min,const double& max,const double& increment) {
	market::key mkey(cid1,cid2);
        auto m=_markets.find(mkey);
        if (m==_markets.end()) {
                return;
        }
        m->second->set_ruler(encode(min),encode(max),encode(increment));
}
void c::set_ruler(int cid1, int cid2, const double& min,const double& max) {
	market::key mkey(cid1,cid2);
        auto m=_markets.find(mkey);
        if (m==_markets.end()) {
                return;
        }
        m->second->set_ruler(encode(min),encode(max));
}

market& c::get_market(int cur1, int cur2) {
	market::key mkey(cur1,cur2);
        return *_markets.find(mkey)->second;
}

id c::new_account(const string& pwd, ostream& err) {
debug(err << L"new_account acc:" << endl);
	if (pwd.size()<8) {
		err << L"Error: E2; password is too short. At least 8 characters are required, no spaces." << endl;
		return 0;
	}
	account* a=new account(*this,pwd);
	a->new_wallet(eur::id);
	a->new_wallet(gbp::id);
	a->new_wallet(usd::id);
	lock_guard<mutex> lock(_mx_accounts);
	_accounts.emplace(a->_id,a);
debug(err << L"  new acc id:" << a->_id << endl);
	return a->_id;
}

void c::new_wallet(const id& acid, int cid) {
	account* a=get_account(acid);
	if (a==0) return;
	a->new_wallet(cid);
	return;
}
account* c::get_account(const id& acid) const {
	lock_guard<mutex> lock(_mx_accounts);
	auto i=_accounts.find(acid);
	if (i==_accounts.end()) { //retrieve from disk
		if (unlikely(!persistence())) return 0;
		if (account::exists(*this,acid)) {
			account* ac=new account(const_cast<engine&>(*this),acid);
			_accounts.emplace(acid,ac);
			i=_accounts.find(acid);
		}
		else {
			return 0;
		}
	}
	return i->second;
}
void c::set_commission_discount(const id& acid, const double&d) {
	account* a=get_account(acid);
	if (a==0) {
		return;
	}
	a->set_commission_discount(d);
}


account* c::auth(const id& acid, const string&pwd, ostream& os) const {
	static const string errmsg=L"Error: E1; account id doesn't exist or password is incorrect.";
	if (acid==_system_account->_id) { //system account, api access forbidden
		os << errmsg << endl;
		return 0;
	}
	account* a=get_account(acid);
	if (a==0) {
		os << errmsg << endl;
		return 0;
	}
	if (!a->check_password(pwd)) {
		os << errmsg << endl;
		return 0;
	}
	return a;
}

void c::new_wallet(const id& acid, const string&pwd, int cid, ostream& os) {
	auto acc=auth(acid, pwd, os);
	if (acc==0) return;
	if (!is_valid(cid)) {
		os << L"Error: E4; unrecognized currency code." << endl;
		return;
	}
	if (!acc->new_wallet(cid)) {
		os << L"Error: E40; wallet already exists." << endl;
	}
}
void c::delete_wallet(const id& acid, const string&pwd, int cid, ostream& os) {
	auto acc=auth(acid, pwd, os);
	if (acc==0) return;
	if (!is_valid(cid)) {
		os << L"Error: E4; unrecognized currency code." << endl;
		return;
	}
	if (!acc->delete_wallet(cid)) {
		os << L"Error: E41; cannot delete an inexistent wallet or a wallet containing funds." << endl;
	}
}

void c::delete_all_wallets(const id& acid, const string&pwd, ostream& os) {
	auto acc=auth(acid, pwd, os);
	if (acc==0) return;
	if (!acc->delete_all_wallets()) {
		os << L"Error: E42; cannot delete wallets containing funds." << endl;
	}
}

void c::deposit(const id& acid, const string&pwd, int cid, const double& amount, ostream& os) {
	ostringstream sos;
	sos << amount;
	string sam=sos.str();
	deposit(acid,pwd,cid,sam,os);
}

void c::deposit(const id& acid, const string&pwd, int cid, const string& amount, ostream& os) {
debug(os << L"deposit acc:" << acid << L" " << curstr(cid) << L" " << amount << endl);

	auto acc=auth(acid, pwd, os);
	if (acc==0) return;
	if (!is_valid(cid)) {
		os << L"Error: E4; unrecognized currency code." << endl;
		return;
	}
	{
		double am=-1;
		istringstream is(amount);
		is >> am;
		if (am<0) {
			os << L"Error: E202; invalid amount " << amount << endl;
			return;
		}
	}
	efp credited=encode(amount);
	if (credited==0) {
		os << L"Error: E202; invalid amount " << amount << endl;
		return;
	}
	acc->deposit(cid,credited);

	os << decode(credited) << L" " << curstr(cid) << L" has been credited to your account." << endl;

	#ifdef MONEY_LEAK_TEST
	reset_check_money();
	#endif

}
void c::list_account(const id& acid, const string&pwd, ostream& os) const {
debug(os << L"list_account acc:" << acid << endl);
	auto acc=auth(acid, pwd, os);
	if (acc==0) return;
	acc->list(os);
}
void c::markets_columns(ostream&os) {
	market::market_columns(os);
}
void c::valid_rates_columns(ostream&os) {
	market::valid_rates_columns(os);
}
void c::valid_rates_simple_columns(ostream&os) {
	market::valid_rates_simple_columns(os);
}
void c::trades_columns(ostream&os)  {
	account::transaction::report_columns(os);
}
/*
void c::matches_columns(ostream&os)  {
	matches::summary::report_columns(os);
}
*/
void c::unmatches_columns(ostream&os) {
	order::report_columns(os);
}
void c::account_columns(ostream&os) {
	account::list_columns(os);
}
void c::liquidity_columns(ostream&os) {
	front::liquidity_columns(os);
}
/*
void c::liquidity_simple_columns(ostream&os) {
	front::liquidity_simple_columns(os);
}
*/



void c::list_markets(ostream& os) const {
	for (auto i:_markets) {
		os << i.second->get_name() << L" " << i.second->_weak_currency << L"/" << i.second->_strong_currency << endl;
	}
}
void c::liquidity(int cid1, int cid2, unsigned int depth, ostream& os) const {
debug(os << L"liquidity " << curstr(cid1) << L" " << curstr(cid2) << L" depth:" << depth << endl);
	market::key mkey(cid1,cid2);
	auto m=_markets.find(mkey);
	if (m==_markets.end()) {
		os << "Error: E5; There is not market for the specified currencies.";
		return;
	}
	m->second->liquidity(depth, os);
}
c::info c::get_info(int cid1, int cid2, efp center, int width) const {
	market::key mkey(cid1,cid2);
	auto m=_markets.find(mkey);
	if (m==_markets.end()) {
		return market::info();
	}
	return move(m->second->get_info(center,width));
}
/*
void c::liquidity_simple(int cid1, int cid2, unsigned int depth, ostream& os) const {
	market::key mkey(cid1,cid2);
	auto m=_markets.find(mkey);
	if (m==_markets.end()) {
		os << "Error: E6; There is not market for the specified currencies.";
		return;
	}
	m->second->liquidity_simple(depth, os);
}

*/
curex::cex::string c::get_market_str(int cid1, int cid2) const {
	ostringstream os;
	os << curstr(cid1) << "/" << curstr(cid2);
	string ans=os.str();
	return move(ans);
}

void c::valid_rates(int cid1, int cid2, ostream& os) const {
debug(os << L"valid_rates " << curstr(cid1) << L" " << curstr(cid2) << endl);
	market::key mkey(cid1,cid2);
	auto m=_markets.find(mkey);
	if (m==_markets.end()) {
		os << "Error: E7; There is not market for the specified currencies.";
		return;
	}
	m->second->valid_rates(os);
}
market::spots c::valid_rates(int cid1, int cid2) const {
	market::key mkey(cid1,cid2);
	auto m=_markets.find(mkey);
	if (m==_markets.end()) {
		return market::spots();
	}
	return m->second->valid_rates();
}
void c::valid_rates_simple(int cid1, int cid2, ostream& os) const {
	market::key mkey(cid1,cid2);
	auto m=_markets.find(mkey);
	if (m==_markets.end()) {
		os << "Error: E8; There is not market for the specified currencies.";
		return;
	}
	m->second->valid_rates_simple(os);
}

void c::status(const id& acid, const string&pwd, ostream& os) const {
debug(os << L"status acc:" << acid << endl);
	auto acc=auth(acid, pwd, os);
	if (acc==0) return;
	_report_status(*acc,os);
}
void c::status_sys_account(ostream& os) const {
debug(os << L"status sys acc" << endl);
	_report_status(*_system_account,os);
}

double c::get_wallet(const id& acid, const string&pwd, int cid, ostream& os) const {
	auto acc=auth(acid, pwd, os);
	if (acc==0) return 0;
	return acc->get_wallet(cid);
}

c::dwallets c::get_wallets(const id& acid, const string&pwd, ostream& os) const {
debug(os << L"get_wallets acc:" << acid << endl);
	auto acc=auth(acid, pwd, os);
	if (acc==0) return dwallets();
	return acc->get_wallets();
}

void c::list_orders(const id& acid, const string&pwd, ostream& os) const {
debug(os << L"list_orders acc:" << acid << L" " << endl);
	auto acc=auth(acid, pwd, os);
	if (acc==0) return;
	acc->list_orders(os);
}
void c::list_trades(const id& acid, const string&pwd, ostream& os) const {
	auto acc=auth(acid, pwd, os);
	if (acc==0) return;
	acc->list_trades(os);
}

void c::withdraw(const id& acid, const string&pwd, int cid, const string& amount, ostream& os) {
	auto acc=auth(acid, pwd, os);
	if (acc==0) return;
	if (!is_valid(cid)) {
		os << L"Error: E9; unrecognized currency code." << endl;
		return;
	}
	{
		double am=-1;
		istringstream is(amount);
		is >> am;
		if (am<0) {
			os << L"Error: E202; invalid amount " << amount << endl;
			return;
		}
	}
	efp debited=encode(amount);
	if (debited==0) {
		os << L"Error: E202; invalid amount " << amount << endl;
		return;
	}
	efp withdrawn=acc->withdraw(cid,debited);
	if (!check_withdrawn(withdrawn,debited,os)) {
		return;
	}

	os << decode(withdrawn) << L" " << curstr(cid) << L" has been withdrawn from your account." << endl;

	#ifdef MONEY_LEAK_TEST
	reset_check_money();
	#endif

}

void c::trade(const id& acid, const string&pwd, const vector<tradeinput>& input, ostream&os) {
	auto acc=auth(acid, pwd, os);
	if (acc==0) return;
	
	for (auto& ti:input) _trade(*acc,ti,false,os);

}
void c::trade_low(const id& acid, const string&pwd, const vector<tradeinput>& input, ostream&os) {
	auto acc=auth(acid, pwd, os);
	if (acc==0) return;
	
	for (auto& ti:input) _trade(*acc,ti,true,os);

}


#include <boost/algorithm/string.hpp>

void c::_trade(account& acc, const tradeinput& input, bool pri_low, ostream&os) {
debug(os << L"trade acc:" << acc._id << L" "; input.trace(os));
	bool buy;
	if (input.code==L"B") buy=true;
	else if (input.code==L"S") buy=false;
	else {
		os << "Error: E88; Invalid code, expected B or S, found " << input.code << endl;
		return;
	}
	{
		double amount=-1;
		istringstream is(input.amount);
		is >> amount;
		if (amount<0) {
			os << L"Error: E202; invalid amount " << input.amount << endl;
			return;
		}
	}
	efp amount=encode(input.amount);
	if (amount==0) {
		os << "Error: E89; Invalid amount " << input.amount << endl;
		return;
	}
	int curid=strcur(input.cur);
	if (curid==0) {
		os << "Error: E90; Invalid amount currency " << input.cur << endl;
		return;
	}
	efp rate=encode(input.rate);
	if (rate==0) {
		os << "Error: E91; Invalid rate value " << input.rate << endl;
		return;
	}

	vector<string> strs;
	boost::split(strs, input.rateunits, boost::is_any_of("/"));
	if (strs.size()!=2) {
		os << "Error: E92; Invalid rate units " << input.rateunits << endl;
		return;
	}
	int weakcur=strcur(strs[0]);	
	if (weakcur==0) {
		os << "Error: E92; Invalid rate units " << input.rateunits << endl;
		return;
	}
	int strongcur=strcur(strs[1]);	
	if (strongcur==0) {
		os << "Error: E92; Invalid rate units " << input.rateunits << endl;
		return;
	}
	
	market::key mkey(weakcur,strongcur);
	auto m=_markets.find(mkey);

	if (m==_markets.end()) {
		os << L"Error: E12; There is not a market for the specified currencies." << endl;
		return;
	}

	#ifdef MONEY_LEAK_TEST
	check_money();
	#endif

	_sell(acc,*m->second,curid,rate,amount,buy,pri_low,os); 

	#ifdef MONEY_LEAK_TEST
	check_money();
	#endif
}

// locks _orders._mx
id c::_sell(account& acc, market& m, int cid, efp limit_rate, const efp& amount, bool buy, bool pri_low, ostream& os) {
	int charge_cur=buy?m.other_cur(cid):cid;
	
	auto wallet=acc.find(charge_cur);
	if (wallet==acc.end()) {
		os << L"Error: E101; A wallet funded with " << curstr(charge_cur) << " is required." << endl;
		return 0;
	}

	market::result r=m.trade(wallet->second,cid,limit_rate,amount,buy,pri_low,os);

	if (r.error) {
		//elog << "ui que error" << endl;
		///TODO rollback actions done
		//os << "Error: E17; h786699954" << endl;
		acc.deposit(cid,amount);
		return 0;
	}
	r.buy=buy;

	//accounting	
	id oid=process(acc, r, os);

	#ifdef ELOG
	check_integrity();
	#endif


	return oid;
}


void c::cancel_order(const id& acid, const string&pwd, const id& order_id, ostream& os) {
	static const string errmsg_notexists=L"Error: E18; Unable to locate the specified order.";

	auto acc=auth(acid, pwd, os);
	if (acc==0) return;

	lock_guard<mutex> lock(_orders._mx);

	auto o=_orders._find(order_id);
	if (o==_orders.end())  {
		os << errmsg_notexists << endl;
		return;
	}
	if (o->second->_account_id!=acid) {
		os << errmsg_notexists << endl;
		return;
	}

	_cancel_order(*acc,*o->second,os);


	//_report_status(*acc,os);

}


void c::_report_status(const account& acc, ostream& os) const {
	acc.list_orders(os);
	acc.list_trades(os);
	acc.list(os);
}



//requires locks: _orders._mx
bool c::_cancel_order(account& acc, order& o, ostream& os) {
	market::key mkey(o._weak_cur,o._strong_cur);
	auto m=_markets.find(mkey);

	order* co=m->second->cancel_order(o._id,os);
	if (co!=0) {
		_orders._erase(co->_id);
		acc.untrack_order(co->_id);
		acc.deposit(co->_paid_cur,co->_value);
		delete co;
		os << "C  " << o._id << endl;
	}
	else {
		os << "Error 555: order not found " << o._id << endl;
		return false;	
	}
	return true;	
}
	

#ifdef ELOG
//requires lock_guard<mutex> lock(_orders._mx);
void c::check_integrity() const {
	elog << endl << "integrity check" << endl;
	vector<order*> ords;
	for (auto& i:_markets) {
		i.second->get_orders(ords);
	}
	elog << ords.size() << " en los mercados" << endl;
	elog << _orders.size() << " en la cache " << endl;
	for (auto i:ords) {
		if (_orders.find(i->_id)==_orders.end()) {
			elog << "order " << i->_id << " no se encuentra en cache" << endl;
		}		
	}
	elog << "end of integrity check" << endl;
}
#endif

void c::track_orders(market& m) {
	vector<order*> ords;
	m.get_orders(ords);
	for (auto o: ords) {
		account* a=get_account(o->_account_id);
		a->track_order(*o);
		_orders.emplace(o->_id, o);
	}	
}

void c::cancel_all_orders(const id& acid, const string&pwd, ostream&os) {
	auto acc=auth(acid, pwd, os);
	if (acc==0) return;
	lock_guard<mutex> lock(_orders._mx);
	while (!acc->_orders.empty()) {
		auto i=acc->_orders.begin();
		if (!_cancel_order(*acc,*i->second,os)) {
			abort();
		}
	}
}

void c::update(const id& acid, const string&pwd, const std::vector<updateinput>& input, ostream&os) {
	auto acc=auth(acid, pwd, os);
	if (acc==0) return;
	for (auto& ti:input) update_order(*acc,ti,os);
	_report_status(*acc,os);
}

void c::updateinput::trace(ostream& os) const {
	string s1,s2,s3;
	ostringstream os1(order_id);
	ostringstream os2(amount);
	ostringstream os3(rate);
	os << "ord:" << os1.str() << L" am:" << os2.str() << L" rate:" << os3.str() << endl;
}
void c::tradeinput::trace(ostream& os) const {
	string s1,s2,s3,s4,s5,s6;
	ostringstream os1(id_of_client);
	ostringstream os2(code);
	ostringstream os3(amount);
	ostringstream os4(cur);
	ostringstream os5(rate);
	ostringstream os6(rateunits);
	os << "tag:" << os1.str() << L" code:" << os2.str() << L" am:" << os3.str() << L" cur:" << os4.str() << L" rate:" << os5.str() << L" " << os6.str() << endl;
}
void c::tradeinput::set_amount(const double& v) {
	amount=decode(encode(v));
}
void c::tradeinput::set_cur(int cid) {
	cur=curstr(cid);
}
void c::tradeinput::set_rate(const efp& r) {
	rate=decode(r);
}

void c::updateinput::set_amount(const double& v) {
	amount=decode(encode(v));
}
void c::updateinput::set_order_id(const id& id_) {
	ostringstream s;
	s << id_;
	order_id=s.str();
}
void c::updateinput::set_rate(const efp& r) {
	rate=decode(r);
}

void c::update_order(account& acc, const updateinput& input, ostream& os) {
debug(os << L"update_order2 acc:" << acc._id << L" "; input.trace(os));
	id order_id=0;
	{
		istringstream is(input.order_id);
		is >> order_id;
		if (order_id==0) {
			os << L"Error: E201; unrecognized Order Id " << input.order_id << endl;
			return;
		}
	}
	{
		double amount=-1;
		istringstream is(input.amount);
		is >> amount;
		if (amount<0) {
			os << L"Error: E202; invalid amount " << input.amount << endl;
			return;
		}
	}
	efp amount=encode(input.amount);
	{
		double rate=-1;
		istringstream is(input.rate);
		is >> rate;
		if (rate==-1) {
			os << L"Error: E203; invalid rate " << input.rate << endl;
			return;
		}
	}
	efp rate=encode(input.rate);

	auto o=_orders.find(order_id);
	if (o==_orders.end())  {
		os << L"Error: E19; Unable to locate the specified order." << endl;
		return;
	}
	order& ord=*o->second;
	if (amount<1) {
		lock_guard<mutex> lock(_orders._mx);
		_cancel_order(acc,ord,os);
		return;
	}
	if (rate!=ord._rate) {
		int curid=ord._buy?ord._received_cur:ord._paid_cur;
//		auto m=_markets.find(market::key(ord._weak_cur,ord._strong_cur));
		market::key mkey(ord._weak_cur,ord._strong_cur);
		auto m=_markets.find(mkey);
		{
		lock_guard<mutex> lock(_orders._mx);
		_cancel_order(acc,ord,os);
		}
		_sell(acc,*m->second,curid,rate,amount,ord._buy,false,os);
		return;
	}
	lock_guard<mutex> lock(_orders._mx);
	_update_order(acc, ord, amount, os);

	#ifdef MONEY_LEAK_TEST
	check_money();
	#endif

}

id c::update_order(const id& acid, const string&pwd, const id& order_id, const string& amount, ostream& os) {
	static const string errmsg_notexists=L"Error: E19; Unable to locate the specified order.";
debug(os << L"update_order acc:" << acid << L" ord:" << order_id << L" " << amount << endl);
	auto am=encode(amount);
	if (am<1) {
		cancel_order(acid, pwd, order_id, os);
		return 0;
	}

	auto acc=auth(acid, pwd, os);
	if (acc==0) return 0;

	lock_guard<mutex> lock(_orders._mx);
	auto o=_orders._find(order_id);
	if (o==_orders.end())  {
		os << errmsg_notexists << endl;
		return 0;
	}
	if (o->second->_account_id!=acid) {
		os << errmsg_notexists << endl;
		return 0;
	}

	const order* neworder=_update_order(*acc, *o->second, am, os);

	#ifdef MONEY_LEAK_TEST
	check_money();
	#endif

	if (neworder!=0) {
		return neworder->_id;
	}
	return 0;
	
}

bool c::check_withdrawn(const efp& withdrawn, const efp& requested, ostream&os) const {
	if (withdrawn==0) {
		os << "Error: E301; No funds in account." << endl;
		return false; 
	}
	if (withdrawn!=requested) {
		os << "Error: E11; Not enough funds in account yo achieve the amount requested. All available amount is used instead (" << decode(withdrawn) << ")." << endl;
		return true; 
	}
	return true; 
}

//requires 	lock_guard<mutex> lock(_orders._mx);
const order* c::_update_order(account& acc, order& o, const efp& amount, ostream& os) {

	efp budget=o.budget_for_update(amount);
	if (budget>0) {
		efp withdrawn=acc.withdraw(o._paid_cur,budget);
		if (!check_withdrawn(withdrawn,budget,os)) {
			return 0;
		}
	}
	else {
		acc.deposit(o._paid_cur,-budget);
	}

	auto m=_markets.find(market::key(o._weak_cur,o._strong_cur));

	auto o2=m->second->update_order(o._id,amount,os);
	o.report(os);
	if (o2!=0) {
		o2->set_data(o._account_id, o._received_cur, o._paid_cur, o._rate, o._weak_cur, o._strong_cur);
		o2->report(os);
		acc.track_order(*o2);
		_orders._emplace(o2->_id, o2);
		return o2;
	}
	return 0;
}
/*
void c::dump(const string& prefix, ostream& os) const {
	os << prefix << L"---------------------------------" << endl;
	os << prefix << L"accounts" << endl;
	dump_accounts(prefix+L"  ",os);
	os << prefix << L"currencies" << endl;
	dump_currencies(prefix+L"  ",os);
	os << prefix << L"markets" << endl;
	dump_markets(prefix+L"  ",os);
//		os << prefix << "orders" << endl;
//		dump_orders(prefix+"  ",os);
}
*/
/*
void c::dump_accounts(const string& prefix, ostream& os) const {
	os << prefix << L"system account" << endl;
	_system_account->dump(prefix+L"  ",os);
	lock_guard<mutex> lock(_mx_accounts);
	for (auto i:_accounts) {
		os << prefix << L"user account " << i.first << endl;
		i.second->dump(prefix+L"  ", os);
	}
}
*/
void c::dump_markets(const string& prefix, ostream& os) const {
	for (auto i:_markets) {
		os << prefix << L"market " << i.second->get_name() << endl;
		i.second->dump(prefix+L"  ", os);
	}
}
/*
void c::dump_orders(const string& prefix, ostream& os) const {
	lock_guard<mutex> lock(_mx);
	for (auto i:_orders) {
		os << prefix << L"order " << i.first << endl;
		i.second.dump(prefix+L"  ",os);
	}
}
*/


id c::process(account& acc, const market::result& r, ostream& os) {
    id ret{0};
	if (r._order!=0) {
		r._order->set_data(acc._id,r._recv_cur, r._paid_cur,r._order_rate,r._weak_cur, r._strong_cur);
		_orders.emplace(r._order->_id, r._order);
		acc.track_order(*r._order);
		r._order->report(os);

//		acc.save_wallets();
		ret=r._order->_id;
	}
	process(acc, r._matches, r._strong_cur, os);
	if (!r._matches._expired_orders.empty()) {
		lock_guard<mutex> lock(_orders._mx);
		for (auto o:r._matches._expired_orders) {
			account* layacc=get_account(o->_account_id);
			layacc->deposit(o->_paid_cur,o->_value);
			layacc->untrack_order(o->_id);
			_orders._erase(o->_id);
			delete o;
		}
	}
	acc.save_wallets();
	return 0;

}	

void c::process(account& acc, const matches& m, int strong_cur, ostream& os) {

	auto acc_tr=account::transaction();

	for (auto& i:m) {
		auto oi=_orders.find(i._lay_order_id); 
		const order* o=oi->second;
		const id& lay_acid=o->_account_id;
		account& layacc=*get_account(lay_acid);

		if (unlikely(acc._id==layacc._id && auto_match_free)) { //no commission taken when auto match
			efp commission_paid=0;
			{
			pay0(layacc,i._lay._received_cur,i._lay._received,0);
			layacc.register_trade(account::tradedata(
				i._lay_order_id,
				i._rate, o->_weak_cur, o->_strong_cur,
				i._lay._cost_cur, i._lay._cost,
				i._lay._received_cur, i._lay._received,
				i._lay._received_cur, commission_paid,
				i._lay._weak, i._lay._buy,true));
			}
			{
			pay0(acc,i._back._received_cur,i._back._received,0);
			acc_tr.register_trade(account::tradedata(
				i._lay_order_id,
				i._rate, o->_weak_cur, o->_strong_cur,
				i._back._cost_cur, i._back._cost,
				i._back._received_cur, i._back._received,
				i._back._received_cur, commission_paid,
				i._back._weak, i._back._buy,false));
			}
		}
		else {
			{
			efp commission_paid=pay(layacc,i._lay._received_cur,i._lay._received);
			layacc.register_trade(account::tradedata(
				i._lay_order_id,
				i._rate, o->_weak_cur, o->_strong_cur,
				i._lay._cost_cur, i._lay._cost,
				i._lay._received_cur, i._lay._received,
				i._lay._received_cur, commission_paid,
				i._lay._weak, i._lay._buy,true));
			}
			{
			efp commission_paid=pay(acc,i._back._received_cur,i._back._received);
			acc_tr.register_trade(account::tradedata(
				i._lay_order_id,
				i._rate, o->_weak_cur, o->_strong_cur,
				i._back._cost_cur, i._back._cost,
				i._back._received_cur, i._back._received,
				i._back._received_cur, commission_paid,
				i._back._weak, i._back._buy,false));

			}
		}
/*
		if (refund>0) {
			pay0(acc,i._back._cost_cur,refund,0);
		}
*/
	}
	if (!m.empty()) {
		acc_tr.recalculate();
		acc_tr.report(os);
		acc.register_transaction(move(acc_tr));
	}
}

efp c::pay(account& acc, int cur, const efp& amount) {
/*
	double profit_perc=decode_dbl(ref_diff>0?ref_diff:0); ///commission proportional to profit, if this account is the one trading at a loss the commission charged is 0 
	//diff of rates is the percentual profit of the trade (if forex rate (between ask and bid rates) represents a profit/loss of 0)
	double profit=decode_dbl(amount)*profit_perc;
	double commission_perc=profit_perc; //curex commission base is the same % applied on the trade profit
	double commission=profit*commission_perc;
*/
	double commission_perc=0.005;  //0.5%
//	double commission_perc=0.13;  //0.5%
//	double commission_perc=0.1;  //10%
	commission_perc*=(1.0-acc._commission_discount);
	double am=decode_dbl(amount);
	double commission=am*commission_perc;
	pair<efp,efp> netcomm=encode(am-commission,commission,amount);
	pay0(acc, cur, netcomm.first, netcomm.second);
	return netcomm.second;
}

void c::pay0(account& acc, int cur, const efp& net, const efp& commission) {
	_system_account->deposit(cur,commission);
	acc.deposit(cur,net);
}

void c::list_currencies(ostream&os) const {
	dump_currencies(L"",os);
}

void c::add_sample_rulers(ostream& os) {
	{
	market* m=&get_market(eur::id,gbp::id);
	m->set_ruler(encode(1.0),encode(2.0));
	}
	{
	market* m=&get_market(usd::id,eur::id);
	m->set_ruler(encode(0.70),encode(1.5));
	}
	{
	market* m=&get_market(aud::id,eur::id);
	m->set_ruler(encode(1.00),encode(2.00));
	}	
	{
	market* m=&get_market(usd::id,gbp::id);
	m->set_ruler(encode(1.00),encode(2.00));
	}	
	{
	market* m=&get_market(aud::id,gbp::id);
	m->set_ruler(encode(1.50),encode(2.50));
	}	
	{
	market* m=&get_market(aud::id,usd::id);
	m->set_ruler(encode(1.00),encode(2.00));
	}	
}

void c::add_sample_liquidity(ostream& os) {
	add_sample_rulers(os);

	string pwd=L"8888888888";
	id nid{0};
	nid=new_account(pwd,os);
	os << "sample liq acc id:" << nid << endl;
	{
	market* m=&get_market(eur::id,gbp::id);
	add_sample_liquidity(nid,*m,1.37,os);
	}

	{
	market* m=&get_market(usd::id,eur::id);
	add_sample_liquidity(nid,*m,1.09,os);
	}
	{
	market* m=&get_market(aud::id,eur::id);
	add_sample_liquidity(nid,*m,1.51,os);
	}	
	{
	market* m=&get_market(usd::id,gbp::id);
	add_sample_liquidity(nid,*m,1.49,os);
	}	
	{
	market* m=&get_market(aud::id,gbp::id);
	add_sample_liquidity(nid,*m,2.08,os);
	}	
	{
	market* m=&get_market(aud::id,usd::id);
	add_sample_liquidity(nid,*m,1.39,os);
	}	
}

#include <random>
#include <cmath>
void c::add_sample_liquidity2(id acid, market& m, int cur, const front::rates& rts, efp front, bool weak, efp amount, ostream&os0) {
ostream os(0);
	os << "adding example liquidity " << m.get_name() << endl;
	account* n=get_account(acid);
	n->deposit(cur,amount);
	vector<efp> spots;
	spots.reserve(rts.size());
	for (auto i:rts) {
		spots.push_back(i);
	}
//	os << "sorting " << spots.size() << "elements..."; os.flush();
	sort(spots.begin(),spots.end());  //mayor volumen en el menor(weak)/mayor(strong)
	vector<double> dist={ 2,13,25,20,15,15,8,2,0,0};
	auto spot=std::lower_bound(spots.begin(), spots.end(), front);
	for (auto i: dist) {
		if (weak) --spot; else ++spot;
		if (spot==spots.end()) break;
		_sell(*n, m, cur, *spot, (efp)((i/100)*((double)amount)), false, false, os);
	}
}

void c::add_sample_liquidity(id acid, market& m, int cur, const front::rates& rts, efp front, bool weak, ostream&os0) {
ostream os(0);
	os << "adding example liquidity " << m.get_name() << endl;
	account* n=get_account(acid);
	efp  wage=100*rate_multiples;
	n->deposit(cur,wage*1000);
	vector<efp> spots;
	spots.reserve(rts.size());
	for (auto i:rts) {
		spots.push_back(i);
	}
//	os << "sorting " << spots.size() << "elements..."; os.flush();
	sort(spots.begin(),spots.end());  //mayor volumen en el menor(weak)/mayor(strong)
//	os << "done" << endl;
//	for (auto i:spots) {
//		os << " rate: " << i << endl;
//	}

	efp width=encode(0.2); // es el gap/2

	efp mean;
	if (weak) {
		mean=front+width; //ma mayoria de ocurrencias en el rate mas alto
	}
	else {
		mean=front-width; //ma mayoria de ocurrencias en el rate mas bajo
	}

//	auto frontit = std::lower_bound(spots.begin(), spots.end(), front);				
//	if (frontit==spots.end()) return;


	std::minstd_rand gen(1729);

	std::normal_distribution<> d(mean,width);  //mean, dispersion
	int co=0;
	map<efp,int> hist;
	for (auto i:spots) {
		hist[i]=0;
	}
	for (int i=0; i<1000; ++i) {
//		os << "getting..."; os.flush();
		efp v=std::round(d(gen));
//		os << "done" << endl;
/*
		if (weak) {
			if (v>*spots.rbegin()) v=*spots.rbegin()-(v-*spots.rbegin()); //fold around the mean
		}
		else {
			if (v<*spots.begin()) v=*spots.begin()+(*spots.begin()-v); //fold around the mean
		}
*/
		if (v>*spots.rbegin()) {
//			os << "ignoring " << v << ">" << *spots.rbegin() << endl;
			continue;
		}
		if (v<*spots.begin()) {
//			os << "ignoring " << v << ">" << *spots.rbegin() << endl;
			continue;
		}
 		//find nearest spot
		auto before = std::lower_bound(spots.begin(), spots.end(), v);				
		if (before==spots.end()) continue;
//		os << "   trading " << curstr(cur) << " @" << *before << endl;
//		os << "trading..."; os.flush();
		_sell(*n, m, cur, *before, wage, false, false, os);
		hist[*before]++;
//		os << "done" << endl;
		++co;
	}
/*
	os << co << " trades made" << endl;
	os << "font " << decode(front) << endl;
	for (auto i:hist) {
		os << "HIST " << decode(i.first) << " " << i.second << endl;
	}
	os << "hist end" << endl;
	list_orders(nid,L"8888888888",os);
//	os << "finished" << endl;
*/

}

void c::add_sample_liquidity(id acid, market& m, const double& refrate, ostream&os) {
	efp front_budget=100000000;
	add_sample_liquidity2(acid, m,m._weak_currency,m._valid_rates,encode(refrate),true,front_budget,os);
	add_sample_liquidity2(acid, m,m._strong_currency,m._valid_rates,encode(refrate),false,front_budget,os);
}



#include <boost/filesystem.hpp>

c::env::nid::nid(const std::string& homebase, const std::string& subdir, const std::string& filename): _next_id(1), persist(false) {
	if (!homebase.empty()) {
		std::ostringstream dir;
		dir << homebase;
	 	if (!subdir.empty()) dir << "/" << subdir;
		home=dir.str();
		persist=true;
	}
	if (likely(persistence())) {
		lock_guard<mutex> lock(_mx);
		boost::filesystem::create_directories(home);
	
		std::ostringstream f;
		f << home << "/" << filename;
		file=f.str();
	
		if (boost::filesystem::exists(file)) {
			load();
		}
		else {
			save();
		}
	}
}
	
id c::env::nid::get() {
	lock_guard<mutex> lock(_mx);
	id i=_next_id++;
	if (likely(persistence())) save();
	return i;
}
void c::env::nid::load() {
	std::ifstream is(file.c_str());
	is >> _next_id;
}
void c::env::nid::save() {
	std::ofstream os(file.c_str());
	os << _next_id << endl;
}

c::env::env(std::string home_):home(home_), nid_account(home_,"account","next"), nid_order(home_,"","next_order"), persist(!home_.empty()) {

}

#include <mstd/algorithm>
void c::dorders::filter_market(const string& market) {
	auto fn=[&](const auto& i) -> bool { return i.market!=market; };
	mstd::filter(*this,fn);
}
void c::dorders::filter_front(int cid) {
	string cur=curstr(cid);
	auto fn=[&](const auto& i) -> bool {
		return (i.buy=='S' && cur!=i.cur) || (i.buy=='B' && cur==i.cur);
	};
	mstd::filter(*this,fn);
}
c::dorders c::dorders::filter_market(const string& market) const {
	dorders copy=*this;
	copy.filter_market(market);
	return move(copy);
}
c::dorders c::dorders::filter_front(int cid) const {
	dorders copy=*this;
	copy.filter_front(cid);
	return move(copy);
}

void c::dorders::dump(ostream& os) const {
	for (auto& i:*this)
		i.dump(os);
}
curex::cex::efp c::dorder::get_encoded_rate() const {
	return encode(rate);
}
double c::dorder::get_rate() const {
	return decode_dbl(encode(rate));
}
double c::dorder::get_amount() const {
	return decode_dbl(encode(amount));
}
int c::dorder::get_cid() const {
	return strcur(cur);
}


void c::dorder::dump(ostream& os) const {
	os << key << L" ";
	os << _id << L" ";
	os << buy << L" ";
	os << amount << L" ";
	os << cur << L" ";
	os << rate << L" ";
	os << market << endl;
}

c::dorders c::get_orders(const id& acid, const string&pwd) const {
	dorders ans;
	ostringstream os;
	list_orders(acid,pwd,os);
	string lo=os.str();
	istringstream is(lo);
	while(!is.eof()) {
		string line;
		getline(is,line);
		dorder o;
		if (line.empty()) continue;
		istringstream lis(line);
		lis >> o.key;
		lis >> o._id;
		lis >> o.buy;
		lis >> o.amount;
		lis >> o.cur;
		lis >> o.rate;
		lis >> o.market;
		ans.emplace_back(move(o));
	}
	return move(ans);
}

//--
void c::dtrades::filter_market(const string& market) {
	auto fn=[&](const auto& i) -> bool { return i.market!=market; };
	mstd::filter(*this,fn);
}
void c::dtrades::filter_front(int cid) {
	string cur=curstr(cid);
	auto fn=[&](const auto& i) -> bool {
		return (i.buy=='S' && cur!=i.cur) || (i.buy=='B' && cur==i.cur);
	};
	mstd::filter(*this,fn);
}
c::dtrades c::dtrades::filter_market(const string& market) const {
	dtrades copy=*this;
	copy.filter_market(market);
	return move(copy);
}
c::dtrades c::dtrades::filter_front(int cid) const {
	dtrades copy=*this;
	copy.filter_front(cid);
	return move(copy);
}

void c::dtrades::dump(ostream& os) const {
	for (auto& i:*this)
		i.dump(os);
}
curex::cex::efp c::dtrade::get_encoded_rate() const {
	return encode(rate);
}
double c::dtrade::get_rate() const {
	return decode_dbl(encode(rate));
}
double c::dtrade::get_amount() const {
	return decode_dbl(encode(amount));
}
double c::dtrade::get_amount2() const {
	return decode_dbl(encode(amount2));
}
double c::dtrade::get_commission() const {
	return decode_dbl(encode(comm));
}
int c::dtrade::get_cid() const {
	return strcur(cur);
}
int c::dtrade::get_cid2() const {
	return strcur(cur2);
}
int c::dtrade::get_cid_comm() const {
	return strcur(cur_comm);
}


void c::dtrade::dump(ostream& os) const {
	os << L"M ";
	os << order_id << L" ";
	os << buy << L" ";
	os << amount << L" ";
	os << cur << L" ";
	os << rate << L" ";
	os << market << L" ";
	os << amount2 << L" ";
	os << cur2 << L" ";
	os << comm << L" ";
	os << cur_comm << endl;

}

c::dtrades c::get_trades(const id& acid, const string&pwd) const {
//M 121 B 80.00000 AUD 1.38995 AUD/USD 57.55600 USD 8.00000 AUD
	dtrades ans;
	ostringstream os;
	list_trades(acid,pwd,os);
	string lo=os.str();
	istringstream is(lo);
	while(!is.eof()) {
		string line;
		getline(is,line);
		dtrade o;
		if (line.empty()) continue;
		istringstream lis(line);
		string key;
		lis >> key;
		assert(key==L"M");
		lis >> o.order_id;
		lis >> o.buy;
		lis >> o.amount;
		lis >> o.cur;
		lis >> o.rate;
		lis >> o.market;
		lis >> o.amount2;
		lis >> o.cur2;
		lis >> o.comm;
		lis >> o.cur_comm;
		ans.emplace_back(move(o));
	}
	return move(ans);
}
//--

c::dliq c::get_liquidity(int cur1, int cur2, unsigned int depth) const {
	dliq ans;
	ostringstream os;
	liquidity(cur1,cur2,depth,os);
	istringstream is(os.str());
	while(!is.eof()) {
		string line;
		getline(is,line);
		if (line.empty()) continue;
		dliqitem o;
		istringstream lis(line);
		lis >> o.rate;
		lis >> o.amount;
		lis >> o.cur;
		ans.emplace_back(move(o));
	}
	return move(ans);
}

void c::dliq::filter_front(int cid) {
	string cur=curstr(cid);
	auto fn=[&](const auto& i) -> bool { return cur!=i.cur; };
	mstd::filter(*this,fn);
}
c::dliq c::dliq::filter_front(int cid) const {
	dliq copy=*this;
	copy.filter_front(cid);
	return move(copy);
}

void c::dliq::dump(ostream& os) const {
	for (auto& i:*this)
		i.dump(os);
}
curex::cex::efp c::dliqitem::get_encoded_rate() const {
	return encode(rate);
}
double c::dliqitem::get_rate() const {
	return decode_dbl(encode(rate));
}
double c::dliqitem::get_amount() const {
	return decode_dbl(encode(amount));
}
int c::dliqitem::get_cid() const {
	return strcur(cur);
}


void c::dliqitem::dump(ostream& os) const {
	os << rate << L" ";
	os << amount << L" ";
	os << cur << L" ";
	os << endl;
}

namespace {
	std::unordered_map<int,efp> psum(const std::unordered_map<int,efp>& c1, const std::unordered_map<int,efp>& c2) {
		std::unordered_map<int,efp> r=c1;
		for (auto& i:r) {
			i.second+=c2.find(i.first)->second;
		}
		return r;
	}
	curex::cex::ostream& operator << (curex::cex::ostream& os, const std::unordered_map<int,efp>& v) {
		for (auto& i:v) {
			os << L"{" << i.first << L":" << i.second << L"} ";
		}
		return os;
	}
	bool operator == (const std::unordered_map<int,efp> a, const std::unordered_map<int,efp>& b) {
		if (a.size()!=b.size()) return false;
		for (auto& i:a) {
			if(b.find(i.first)->second!=i.second) return false;
		}
		return true;
	}
}

#ifdef MONEY_LEAK_TEST

void c::reset_check_money() {
	num_checks=-1;
	check_money();
}

void c::check_money() const {
	num_checks++;
	ostream os(0);
	if (num_checks==0) {
		_g.clear();
		_g.emplace(eur::id,0);
		_g.emplace(gbp::id,0);
		_g.emplace(aud::id,0);
		_g.emplace(usd::id,0);
		total_money(_g,os);
		return;
	}
	std::unordered_map<int,efp> g1;
		g1.emplace(eur::id,0);
		g1.emplace(gbp::id,0);
		g1.emplace(aud::id,0);
		g1.emplace(usd::id,0);
	total_money(g1,os);
	for (auto& i:g1) {
		efp a=_g.find(i.first)->second;
		efp b=i.second;
		if(a!=b) {
			wcout << _g << endl;
			wcout << g1 << endl;
			assert(false);
		}
	}
}
#endif

void c::total_money(ostream& os) const {
	std::unordered_map<int,efp> g;
	g.emplace(eur::id,0);
	g.emplace(gbp::id,0);
	g.emplace(aud::id,0);
	g.emplace(usd::id,0);
	total_money(g,os);
}

void c::stats::t::dump(ostream& os) const {
	for (auto& i:*this)
		os << i.first << L" " << i.second << endl;
}
void c::stats::t::dump_line(std::ostream& os, int cur) const {
	auto i=find(cur);
	if (i==end()) { os << "0 "; return; }
	os << i->second << " ";
}
void c::stats::dump(ostream& os) const {
		os << L"money in orders "; money_in_orders.dump(os);
		os << L"money in accounts "; money_in_accounts.dump(os);
		os << L"money in system account "; money_in_system_account.dump(os);
		os << L"money in exchange "; money_in_exchange.dump(os);
}
void c::stats::dump_line(std::ostream& os, int cur) const {
		money_in_orders.dump_line(os,cur);
		money_in_accounts.dump_line(os,cur);
		money_in_system_account.dump_line(os,cur);
		money_in_exchange.dump_line(os,cur);
}


void c::stats::t::add(int cur, double v) {
	auto i=find(cur);
	if (i==end()) {
		emplace(cur,v);
		return;
	}
	i->second+=v;
}

c::stats::t c::stats::t::operator+(const t& other) const {
	t sum=*this;
	for (auto& i:other) {
		sum.add(i.first,i.second);
	}
	return sum;
	
}


c::stats c::get_stats() const {
	stats r;
	for (const auto& a:_accounts) {
		if (a.first==get_system_account_id()) continue;
		for (const auto& w:*a.second) {
			r.money_in_accounts.add(w.first,decode_dbl(w.second.available));
		}
	}

	for (const auto& o:_orders) {
		auto cv=o.second->get_value();
		r.money_in_orders.add(cv.first,decode_dbl(cv.second));
	}

	for (const auto& w:*_system_account) {
		r.money_in_system_account.add(w.first,decode_dbl(w.second.available));
	}

	r.money_in_exchange=r.money_in_system_account+r.money_in_orders+r.money_in_accounts;
	return move(r);
}

void c::total_money(std::unordered_map<int,efp>& g, ostream& os) const {
	for (const auto& a:_accounts) {
		for (const auto& w:*a.second) {
			g.find(w.first)->second+=w.second.available;
		}
	}
	std::unordered_map<int,efp> go1=g;
	std::unordered_map<int,efp> go2=g;
	std::unordered_map<int,efp> go3=g;

	//go1  orders in engine cache
	for (const auto& o:_orders) {
		auto cv=o.second->get_value();
		go1.find(cv.first)->second+=cv.second;
	}
	os << "orders in engine cache: " << endl;
	os << psum(g,go1) << endl;

	//go2  orders in accounts
	for (const auto& a:_accounts) {
		for (const auto& o:a.second->_orders) {
			auto cv=o.second->get_value();
			go2.find(cv.first)->second+=cv.second;
		}
	}
	os << "orders in accounts: " << endl;
	os << psum(g,go2) << endl;

	//go3  waitiing queues cached value
	for (const auto& m:_markets) {
		{
		auto&v=go3.find(m.second->_weak_front.get_cur())->second;
		for (const auto& q:m.second->_weak_front.get_queues_by_rate()) {
			v+=q.second->get_value();
		}
		}
		{
		auto&v=go3.find(m.second->_strong_front.get_cur())->second;
		for (const auto& q:m.second->_strong_front.get_queues_by_rate()) {
			v+=q.second->get_value();
		}
		}
	}
	os << "orders as queues cached value: " << endl;
	os << psum(g,go3) << endl;

	assert(go1==go2);
	assert(go3==go2);
	g=go3;
}
