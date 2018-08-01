#include "waiting_queue.h"
#include "order.h"
#include <iomanip>
#include <iostream>
#include <boost/filesystem.hpp>
#include <cassert>
#include "engine.h"


using namespace curex::cex;
using namespace std;
typedef curex::cex::waiting_queue c;

waiting_queues::waiting_queues(engine&e,const efp& r, bool invert_priority, const std::string& parentdir):
	high(new waiting_queue(e,r,invert_priority,parentdir)) { //no poner sufijo preservar datos demo

	std::ostringstream os;
	os << high->home << "/low";
	low=new waiting_queue(e,r,invert_priority,os.str());

}

waiting_queues::~waiting_queues() {
	delete low;
	delete high;
}

waiting_queues::info waiting_queues::get_info() const {
	return move(info{high->get_info(),low->get_info()});
}

void waiting_queues::info::write(std::ostream& os) const {
	high.write(os);
	os << " ";
	low.write(os);
	os << endl;
}

efp waiting_queues::get_value() const {
	return high->get_value()+low->get_value();
}
bool waiting_queues::has_value() const {
	if (high->get_value()!=0) return true;
	if (low->get_value()!=0) return true;
	return false;
}
const efp& waiting_queues::get_rate() const {
	return high->_rate;
}
void waiting_queues::get_orders(vector<order*>& ords) const {
	high->get_orders(ords);
	low->get_orders(ords);
}
void waiting_queues::dump(const string& prefix, ostream&os) const {
	high->dump(prefix,os);
	ostringstream pfx;
	pfx << prefix << "low ";
	low->dump(pfx.str(),os);
}
order* waiting_queues::new_order(const efp& amount, bool weak, bool buy, ostream& os) {
	return high->new_order(amount,weak,buy,os);
}
order* waiting_queues::new_order_low(const efp& amount, bool weak, bool buy, ostream& os) {
	auto o=low->new_order(amount,weak,buy,os);
	if (unlikely(o==0)) return 0;
	lock_guard<mutex> lock(_mx_hash_low);
	_hash_low.emplace(o->_id);
	return o;
}
order* waiting_queues::cancel_order(const id& id_, bool save_, ostream& os) {
	lock_guard<mutex> lock(_mx_hash_low);
	if (_hash_low.find(id_)==_hash_low.end())
	return high->cancel_order(id_,save_,os);
	auto o=low->cancel_order(id_,save_,os);
	if (o!=0) _hash_low.erase(o->_id);
	return o;
}
order* waiting_queues::update_order(const id& id_, const efp& amount, ostream& os) {
	lock_guard<mutex> lock(_mx_hash_low);
	if (_hash_low.find(id_)==_hash_low.end())
	return high->update_order(id_,amount,os);
	auto o=low->update_order(id_,amount,os);
	if (o!=0) _hash_low.emplace(o->_id);
	return o;
}

std::pair<bool,efp> waiting_queues::match(wallet& w, matches& m, const efp& amount,int cur, int lay_cur, bool weak, bool buy, ostream& os) {
	auto ans=high->match(w,m,amount,cur,lay_cur,weak,buy,os);
	if (ans.second==0) return ans;
	return low->match(w,m,ans.second,cur,lay_cur,weak,buy,os);
	
}
void waiting_queues::save() const {
	high->save();
	low->save();
}
void waiting_queues::load() {
	high->load();
	low->load();
}

///---------------------------------------------------------------------
c::waiting_queue(engine&e_,const efp& r, bool invert_priority, const std::string& parentdir): _rate(r), _invert_priority(invert_priority),e(e_) {
	if (likely(e.persistence())) {
		home=parentdir;
		assert(!home.empty());
		boost::filesystem::create_directories(home);
	}
}

c::~waiting_queue() {
	lock_guard<mutex> lock1(_mx);
	lock_guard<mutex> lock2(_mx_hash);
	lock_guard<mutex> lock3(_mx_value);
	_hash.clear();
	for (auto i:*this) {
		delete i;
	}
}


void c::get_orders(vector<order*>& ords) const {
	lock_guard<mutex> lock(_mx);
	ords.reserve(ords.size()+size());
	for (auto& i:*this) {
		ords.push_back(i);
	}

}
void c::dump(const string& prefix, ostream&os) const {
	lock_guard<mutex> lock(_mx);
	{
	lock_guard<mutex> lock(_mx_value);
	os << prefix << decode(_rate) << L" : {"<<decode(_value)<<L"} queue: ";
	}
	for (auto i:*this) {
		os << L"[" << i->_id << L"-" << decode(i->_value) << L"] ";
	}
	os << endl;
}
order* c::new_order(const efp& amount, bool weak, bool buy, ostream& os) {
	if (amount<1) {
		os << "Error: E29; incorrect amount. l43986759" << endl;
		return 0;
	}
	order* o=new order(e,amount,weak,buy, *this);

	lock_guard<mutex> lock1(_mx);
	emplace_back(o);

	lock_guard<mutex> lock2(_mx_hash);
	_hash.emplace(o->_id,--end());

	lock_guard<mutex> lock3(_mx_value);
	_value+=o->_value;

	return o;
}
order* c::cancel_order(const id& id_, bool save_, ostream& os) {
	lock_guard<mutex> lock1(_mx);
	return _cancel_order(id_, save_, os);
}

order* c::_cancel_order(const id& id_, bool save_, ostream& os) {
	lock_guard<mutex> lock2(_mx_hash);
	auto i=_hash.find(id_);
	if (i==_hash.end()) {
		os << "Error: E30; incorrect order id. l43986750 - " << id_ << endl;
		return 0;
	}
	order* o=*i->second;

	{
	lock_guard<mutex> lock(_mx_value);
	_value-=o->_value;
	}

	erase(i->second); //fuera de la cola

	_hash.erase(o->_id);

	if (save_ && likely(e.persistence())) _save();
	return o;
}
order* c::update_order(const id& id_, const efp& amount, ostream& os) { //returns new order or 0
	if (amount<1) {
		os << "Error: E31; incorrect amount. l43986749" << endl;
		return 0;
	}
	lock_guard<mutex> lock1(_mx);
	lock_guard<mutex> lock2(_mx_hash);
	auto i=_hash.find(id_);
	if (i==_hash.end()) {
		os << "Error: E32; incorrect order id. l43986751" << endl;
		return 0;
	}
	order* o;
	{	
		lock_guard<mutex> lock3(_mx_value);
		_value-=(*i->second)->_value;
		o=(*i->second)->update(amount);
		_value+=(*i->second)->_value;
		if (o!=0) _value+=o->_value;
	}
	if (o!=0) {
		emplace_back(o);
		_hash.emplace(o->_id,--end());
	}
	if (likely(e.persistence())) _save();
	return o;
}

/*
	if (buy) {//transform to sell in the other front. buy 100 eur @ 1.50 eur/gbp = sell 100/1.50 gbp
							//buy 100 gbp @ 1.50 eur/gbp = sell 100*1.50 eur
//os << "Converted buy " << decode(amount) << curstr(curid) << endl;
		if (curid==weakcur) {
			amount=encode(decode_dbl(amount)/decode_dbl(rate));
			curid=strongcur;
//os << "back conversion gives amount " << decode(encode(decode_dbl(amount)*decode_dbl(rate))) << endl;
		}
		else {
			amount=encode(decode_dbl(amount)*decode_dbl(rate));
			curid=weakcur;
//os << "back conversion gives amount " << decode(encode(decode_dbl(amount)/decode_dbl(rate))) << endl;
		}
//os << " in sell " << decode(amount) << curstr(curid) << endl;
	}
*/

const efp& c::get_value() const {
	lock_guard<mutex> lock(_mx_value);
	return _value;
}

c::info c::get_info() const {
	info i;
	{
	lock_guard<mutex> lock(_mx_value);
	i.value=_value;
	}
	{
	lock_guard<mutex> lock(_mx);
	i.size=size();
	}
	return move(i);
}
void c::info::write(std::ostream& os) const {
	os << size << " " << decode_std(value);
}

pair<bool,efp> c::match(wallet& w, matches& m, const efp& amount,int cur, int lay_cur, bool weak, bool buy, ostream& os) {
//weak front:
	//buy true, EUR; cur GBP, laycur EUR
	//buy false GBP; cur GBP, laycur EUR
	efp amount_left=amount;

	lock_guard<mutex> lock(_mx);
	while (true) {
		if (empty()) break;

		if (!order::could_match_provided_big_ordervalue(w,_rate,amount_left,weak,buy,os)) break;
		order* o=*begin();
		std::pair<bool,efp> ans;
		{
			lock_guard<mutex> lock(_mx_value);
			_value-=o->_value;
			ans=o->match(w, m,_rate,amount_left,cur,lay_cur,weak,buy,os);
			_value+=o->_value;
		}
		amount_left=ans.second;
		if (ans.first) {
			m._updated_orders.push_back(o->_id);
		}
		if (!order::could_match_provided_big_amount(_rate,o->_value,weak,buy)) {
			order* oc=_cancel_order(o->_id, false, os);
			m._expired_orders.emplace_back(oc);
		}
	}
	if (likely(e.persistence())) _save();
	if (empty()) {
		return pair<bool,efp>(false,amount_left);
	}
	return pair<bool,efp>(true,amount_left);
}

bool c::compare(const efp& otherrate) const {
	return _invert_priority?_rate <= otherrate:_rate >= otherrate;
} 

void c::save() const {
	if (unlikely(!e.persistence())) return;
	lock_guard<mutex> lock(_mx);
	_save();
}
void c::_save() const {
	std::ostringstream file;
	file << home << "/" << _rate;
	if (empty()) {
		if (boost::filesystem::exists(file.str())) {
			boost::filesystem::remove(file.str());
		}
	}
	else {
		std::ofstream os(file.str());
		for (auto i:*this) {
			i->write(os);
		}
	}
}
//curex::cex::string strcast(const std::string& o);

void c::load() {
	std::ostringstream file;
//wcout << L"queue home: " << strcast(home) << endl;
	file << home << "/" << _rate;
	lock_guard<mutex> lock1(_mx);
	lock_guard<mutex> lock2(_mx_hash);
	lock_guard<mutex> lock3(_mx_value);
	auto fn=file.str();
	if (boost::filesystem::exists(fn)) {
		std::ifstream is(file.str());
		while(!is.eof()) {
			std::string line;
			getline(is,line);
			if (line.empty()) continue;
			order* o=new order(e,line,*this);
			emplace_back(o);
			_hash.emplace(o->_id,--end());
			_value+=o->_value;
		}
	}
}


