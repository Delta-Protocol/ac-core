#include "front.h"
#include <memory>
#include <iostream>
#include <map>
#include <unordered_map>
#include <list>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include "order.h"
#include "engine.h"

using namespace curex::cex;
using namespace std;
typedef curex::cex::front c;

c::front(engine&e_,int curid, bool invert_priority): _curid(curid), _invert_priority(invert_priority), e(e_) {
}
c::~front() {
	_queues_by_order.clear();
	_queues_by_rate.clear();
	while(!B::empty()) {
		delete B::top();
		B::pop();
	}
	//now attic is deleted
}

void c::set_rates(const rates& rs) {
	lock_guard<mutex> lock(_valid_rates_mx);
	cancel_orders_not_in(rs);
	_valid_rates=&rs;
}

void c::cancel_orders_not_in(const rates& rs) {
//cuidado que esta cogido el lock de valid rates
}
void c::liquidity_columns(ostream&os) {
	os << "<rate> <available volume> <currency id>" << endl;
}
void c::liquidity(unsigned int depth, ostream& os) const {
	map<efp,efp> m;
	{
		lock_guard<mutex> lock(_queues_by_rate_mx);
		for (auto& i:_queues_by_rate) {
			auto q=i.second;
			m.emplace(q->get_rate(),q->get_value());
		}
	}

	if (_invert_priority) { //strong front
		for (auto i=m.begin();i!=m.end(); ++i) {
			if (depth--==0) break;
			os << decode(i->first) << L" " << decode(i->second) << " " << _curid << std::endl;
		}
	}
	else {
		auto i=m.begin();
		if (m.size()>depth) advance(i,m.size()-depth);
		for (;i!=m.end(); ++i)
		os << decode(i->first) << L" " << decode(i->second) << " " << _curid << std::endl;
	}
}

c::info c::get_info() const {
	info r;
	r.cur=_curid;
	{
		lock_guard<mutex> lock(_queues_by_rate_mx);
		for (auto& i:_queues_by_rate) {
			auto q=i.second;
			r.emplace(q->get_rate(),q->get_info());
		}
	}
	return move(r);
}

void c::info::write(std::ostream& os) const {
	for (auto& i: *this) {
		os << decode_std(i.first) << " " << cur << " ";
		i.second.write(os);
	}
}

void c::get_orders(vector<order*>& ords) const {
	vector<Q*> v;
	{
		lock_guard<mutex> lock(_queues_by_rate_mx);
		v.reserve(_queues_by_rate.size());
		for (auto& q:_queues_by_rate) {
			v.push_back(q.second);
		}
	}
	for (auto& q:v) {
		q->get_orders(ords);
	}
}

void c::dump(const string& prefix, ostream& os) const {
	{
		lock_guard<mutex> lock(_queues_by_rate_mx);
		for (auto& i:_queues_by_rate) {
			i.second->dump(prefix+L"  ",os);
		}
	}
	lock_guard<mutex> lock(_mx);
	if (!B::empty()) {
	os << prefix << L"highest priority queue is " << B::top()->get_rate() << std::endl;
	}
	else {
	os << prefix << L"empty front " << std::endl;
	}
}

order* c::new_order_low(const efp& rate, const efp& amount, bool weak, bool buy, ostream& os) {
	Q* q=get_queue_rate(rate,os);
	if (q==0) {
		return 0;
	}
	order* o=q->new_order_low(amount,weak,buy,os);
	if (o==0) {
		os << "Error: E24; unknown g60599490" << endl;
		return 0;
	}
	lock_guard<mutex> lock(_queues_by_order_mx);
	_queues_by_order.emplace(o->_id,q);
	return o;
}

order* c::new_order(const efp& rate, const efp& amount, bool weak, bool buy, ostream& os) {
	Q* q=get_queue_rate(rate,os);
	if (q==0) {
		return 0;
	}
	order* o=q->new_order(amount,weak,buy,os);
	if (o==0) {
		os << "Error: E24; unknown g60599499" << endl;
		return 0;
	}
	lock_guard<mutex> lock(_queues_by_order_mx);
	_queues_by_order.emplace(o->_id,q);
	return o;
}

order* c::cancel_order(const id& id_, ostream& os) {
	Q* q=get_queue_id(id_);
	if (q==0) {
		return 0; //order is not here 
	}

	order* o=q->cancel_order(id_,true,os);
	if (o!=0) {
		lock_guard<mutex> lock(_queues_by_order_mx);
		_queues_by_order.erase(o->_id);
		trim();  //we might be leaving the highest priority queue empty
	}
	return o;
}

bool c::has_order(const id& orderid) const {
	lock_guard<mutex> lock(_queues_by_order_mx);
	return _queues_by_order.find(orderid)!=_queues_by_order.end();
}

order* c::update_order(const id& id_, const efp& amount, ostream& os) {
	if (amount<1) {
		os << "Error: E26; incorrect amount. u76943032" << endl;
		return 0; //dont let cancel it or will not be accounted
	}

	Q* q=get_queue_id(id_);
	if (q==0) {
		return 0; //not here
	}
	order* o=q->update_order(id_,amount,os);
	if (o!=0) {
		lock_guard<mutex> lock(_queues_by_order_mx);
		_queues_by_order.emplace(o->_id,q);
	}
	return o;
}

bool c::test_match(const efp& limit_rate) const {
	Q* prevq=0;
	while (true) {
		Q* q;
		{
		lock_guard<mutex> lock(_mx);
		if (B::empty()) break;
		q=B::top();
		}
		if (q==prevq) break;
		prevq=q;
		if (!q->high->compare(limit_rate)) break;
		if(q->has_value()) return true;
	}
	return false;
}

matches c::match(wallet& w, const efp& limit_rate, const efp& amount, int cur, int lay_cur, bool buy, ostream& os) {
	matches m;
	efp amount_left=amount;
	Q* prevq=0;
	while (true) {
		Q* q;
		{
		lock_guard<mutex> lock(_mx);
		if (B::empty()) break;
		q=B::top();
		}
		if (q==prevq) break;
		prevq=q;
		if (!q->high->compare(limit_rate)) break;
		auto ans=q->match(w,m,amount_left,cur,lay_cur, !_invert_priority, buy, os);
		trim();
		amount_left=ans.second;
	}
	{
	lock_guard<mutex> lock(_queues_by_order_mx);
	for (auto& i:m._expired_orders) {
		_queues_by_order.erase(i->_id);
	}
	}

	m.unmatched=amount_left;
	m.cost_rounding=amount_left-m.unmatched;
	return move(m);
}

c::Q* c::get_queue_rate(const efp& r, ostream& os) {
	assert(_valid_rates!=0);
	Q* q;
	{
		lock_guard<mutex> lock(_queues_by_rate_mx);
		auto i=_queues_by_rate.find(r);
		if (i!=_queues_by_rate.end()) return i->second;
		{
			lock_guard<mutex> lock(_valid_rates_mx);
			auto i=_valid_rates->find(r);
			if (i==_valid_rates->end()) {
				os << "Error: E28; invalid rate " << decode(r) << endl;
				return 0;
			}
			q=new Q(e,*i,_invert_priority,home);
		}
		
		_queues_by_rate.emplace(q->get_rate(),q);
	}
	lock_guard<mutex> lock(_mx);
	B::emplace(q);
	return q;
}

const c::Q* c::get_queue_rate(const efp& r) const {
	lock_guard<mutex> lock(_queues_by_rate_mx);
	auto i=_queues_by_rate.find(r);
	if (i!=_queues_by_rate.end()) return i->second;
	return 0;
}

c::Q* c::get_queue_id(const id& id_) {
	lock_guard<mutex> lock(_queues_by_order_mx);
	auto i=_queues_by_order.find(id_);
	if (i!=_queues_by_order.end()) return i->second;
	return 0;
}

void c::trim() {
	lock_guard<mutex> lock(_mx);
	while(true) {
		if (B::empty()) break;
		Q* q=B::top();
		if (q->has_value()) break;
		lock_guard<mutex> lock(_queues_by_rate_mx);
		_queues_by_rate.erase(_queues_by_rate.find(q->get_rate()));
		_attic.delete_later(q); //leave alive untill all threads are finished with it
		B::pop(); //remove from the front, no new threads will deal with it anymore
	}
}
#include <boost/filesystem.hpp>

namespace fs=boost::filesystem;

void c::load() {
  lock_guard<mutex> lock(_mx);
  if (!empty()) {
	cerr << "Error 676; load: unexpected non empty front" << endl;
	exit(1);
	return;
  }
  fs::create_directories(home);

  fs::directory_iterator end_iter;
  for( fs::directory_iterator dir_iter(home) ; dir_iter != end_iter ; ++dir_iter) {
    if (fs::is_regular_file(dir_iter->status()) ) {
      	auto p=dir_iter->path().filename();
	std::istringstream is(p.string().c_str());
	efp rate{0};
	is >> rate;
//cout << "creating queue for rate " << rate << endl;
	Q* q=new Q(e,rate,_invert_priority,home);
	B::emplace(q);
	{
		lock_guard<mutex> lock(_queues_by_rate_mx);
		_queues_by_rate.emplace(q->get_rate(),q);
	}
	if (likely(e.persistence())) q->load();
	{
		lock_guard<mutex> lock(_queues_by_order_mx);
		vector<order*> ords;
		q->get_orders(ords);
		for (auto i: ords) {
			_queues_by_order.emplace(i->_id,q);
		}
	}
    }
  }


}

