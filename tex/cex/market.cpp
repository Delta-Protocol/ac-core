#include "market.h"
#include "order.h"
#include "engine.h"
#include <cassert>

using namespace curex::cex;
using namespace std;
typedef curex::cex::market c;
c::market(engine&e_,id weakcur,id strongcur): _strong_currency(strongcur), _weak_currency(weakcur), _weak_front(e_,weakcur,false), _strong_front(e_,strongcur,true),e(e_) {
	if (e.persistence()) {
		std::ostringstream os;
		os << e._env.home << "/market/" <<  _weak_currency << "_" << _strong_currency;
		{
			std::ostringstream os2;
			os2 << os.str() << "/" << _weak_currency;
			_weak_front.home=os2.str();
	//cout << "_weak_front.home " << _weak_front.home << endl;

		}
		os << "/" << _strong_currency;	
		_strong_front.home=os.str();
	//cout << "_strong_front.home " << _strong_front.home << endl;

		_weak_front.load();
		_strong_front.load();
	}	
}

void c::set_ruler(const efp& min,const efp& max) {
	set_ruler(min,max,_ruler_def._increment);
}
void c::set_ruler(const efp& min,const efp& max,const efp& increment) {
	_ruler_def.update(min,max,increment);
	_valid_rates=_ruler_def.generate_rates();
	_weak_front.set_rates(_valid_rates);
	_strong_front.set_rates(_valid_rates);

	_valid_rates_sorted.clear();
	_valid_rates_sorted.reserve(_valid_rates.size());
	for (const auto& r:_valid_rates) {
		_valid_rates_sorted.emplace_back(r);
	}
	std::sort(_valid_rates_sorted.begin(),_valid_rates_sorted.end());
//void c::spots::sort() {
//}

}

void c::ruler_def::update(const efp& min,const efp& max,const efp& increment) {
	_min=min;
	_max=max;
	_increment=increment;
	_min-=_min%_increment; //adjust to be multiple of increment
	_max-=_max%_increment;
	if (_min<_increment) _min=_increment;
	if (_max<_min) _max=_min;
}
efp c::ruler_def::snap(efp v) const {
	v-=v%_increment; //adjust to be multiple of increment
	if (v<_min) v=_min;
	if (v>_max) v=_max;
	return v;
}

front::rates c::ruler_def::generate_rates() const {
	front::rates ans;
	ans.reserve(1+(_max-_min)/_increment);
	for (auto r=_min; r<=_max; r+=_increment) {
		ans.emplace(r);
	}
	return move(ans);
}

void c::get_orders(vector<order*>& ords) const {
	_weak_front.get_orders(ords);
	_strong_front.get_orders(ords);
}

#include "wallet.h"

c::result c::trade_weak(wallet& w, const efp& limit_rate, const efp& amount, bool buy, bool pri_low, ostream& os) {
	result r;
	if (unlikely(pri_low)) {
		if (_strong_front.test_match(limit_rate)) {
			r._order=0;
			r._order_rate=0;
			//r._refund=0;
			r._paid_cur=_weak_currency;
			r._recv_cur=_strong_currency;
			r._weak_cur=_weak_currency;
			r._strong_cur=_strong_currency;
			return std::move(r);
		}
		r._matches.unmatched=amount;
	}
	else {
		r._matches=_strong_front.match(w,limit_rate,amount,_weak_currency,_strong_currency, buy, os);
	}
	if (r._matches.unmatched) {
		if (order::could_match_provided_big_ordervalue(w,limit_rate,r._matches.unmatched,true,buy,os)) {
			if (buy) {
				r._matches.unmatched=encode(decode_dbl(r._matches.unmatched)*decode_dbl(limit_rate));
			}
			if (w.available<r._matches.unmatched) {
				r._matches.unmatched=w.available;
			}
			order* no;
			if (unlikely(pri_low)) no=_weak_front.new_order_low(limit_rate,r._matches.unmatched,true,buy,os);
			else no=_weak_front.new_order(limit_rate,r._matches.unmatched,true,buy,os);
			if (unlikely(no==0)) {
				os << "Error: E33; unexpected null order arrived." << endl;
				r.error=true;
				return r;
			}
			w.withdraw(r._matches.unmatched);
			r._order=no;
			r._order_rate=limit_rate;

		}
		else {
			r._order=0;
			r._order_rate=0;
			//r._refund=0;
			//r._refund+=r._matches.unmatched;
		}
	}
	r._paid_cur=_weak_currency;
	r._recv_cur=_strong_currency;
	r._weak_cur=_weak_currency;
	r._strong_cur=_strong_currency;
	return std::move(r);
}


c::result c::trade_strong(wallet& w, const efp& limit_rate, const efp& amount, bool buy, bool pri_low, ostream& os) {
	result r;
	if (unlikely(pri_low)) {
		if (_weak_front.test_match(limit_rate)) {
			r._order=0;
			r._order_rate=0;
			//r._refund=0;
			r._paid_cur=_strong_currency;
			r._recv_cur=_weak_currency;
			r._weak_cur=_weak_currency;
			r._strong_cur=_strong_currency;
			return std::move(r);
		}
		r._matches.unmatched=amount;
	}
	else {
		r._matches=_weak_front.match(w,limit_rate,amount, _strong_currency, _weak_currency, buy, os);
	}
	if (r._matches.unmatched>0) {
		if (order::could_match_provided_big_ordervalue(w,limit_rate,r._matches.unmatched,false,buy,os)) {
			if (buy) {
				r._matches.unmatched=encode(decode_dbl(r._matches.unmatched)/decode_dbl(limit_rate));
			}
			if (w.available<r._matches.unmatched) {
				r._matches.unmatched=w.available;
			}
			order* no;
			if (unlikely(pri_low)) no=_strong_front.new_order_low(limit_rate,r._matches.unmatched,false,buy,os);
			else no=_strong_front.new_order(limit_rate,r._matches.unmatched,false,buy,os);
			if (unlikely(no==0)) {
				os << "Error: E33; (2) unexpected null order arrived." << endl;
				r.error=true;
				return r;
			}
			w.withdraw(r._matches.unmatched);
			r._order=no;
			r._order_rate=limit_rate;
		}
		else {
			r._order=0;
			r._order_rate=0;
			//r._refund=0;
			//r._refund+=r._matches.unmatched;
		}
	}
	r._paid_cur=_strong_currency;
	r._recv_cur=_weak_currency;
	r._weak_cur=_weak_currency;
	r._strong_cur=_strong_currency;
	return std::move(r);
}

c::result c::trade(wallet& w, id cur, const efp& limit, const efp& amount, bool buy, bool pri_low, ostream& os) {
	if (cur==_strong_currency) {
		if (buy) {
			return std::move(trade_weak(w,limit,amount,buy,pri_low,os));
		}
		else {
			return std::move(trade_strong(w,limit,amount,buy,pri_low,os));
		}
	}
	else {
		if (buy) {
			return std::move(trade_strong(w,limit,amount,buy,pri_low,os));
		}
		else {
			return std::move(trade_weak(w,limit,amount,buy,pri_low,os));
		}
	}
}

order* c::update_order(const id& order_id, const efp& amount, ostream& os) {
	if (_weak_front.has_order(order_id)) {
		return _weak_front.update_order(order_id,amount, os);
	}
	else {
		return _strong_front.update_order(order_id,amount, os);
	}
}

order* c::cancel_order(const id& order_id, ostream& os) {
	if (_weak_front.has_order(order_id)) {
		return _weak_front.cancel_order(order_id, os);
	}
	else {
		return _strong_front.cancel_order(order_id, os);
	}

}

curex::cex::string c::get_name() const {
	ostringstream os;
	os << curstr(_weak_currency) << L"/" << curstr(_strong_currency);
	return os.str();	
}

void c::dump(const string& prefix, ostream& os) const {
	os << prefix << curstr(_weak_currency) << L" front:" << std::endl;
	_weak_front.dump(prefix+L"  ",os);
	os << prefix << curstr(_strong_currency) << L" front:" << std::endl;
	_strong_front.dump(prefix+L"  ",os);
}

void c::result::dump(const string& prefix, ostream& os) const {
	_matches.dump(prefix,os);
	if (_order!=0) {
		os << prefix << L"unmatched order id " << _order->_id << std::endl;
	}
}
/*
void c::result::report(ostream& os) const {
	// matched
	// unmatched order_id
	_matches.report(_paid_cur,_recv_cur,_weak_cur,_strong_cur,os);
	if (_order!=0) {
		_order->report(_paid_cur,_weak_cur,_strong_cur,_order_rate,buy,os);
	}
}
*/

void c::market_columns(ostream& os) {
	os << L"<market name> <currency id>/<currency id>" << endl;
}


void c::liquidity(unsigned int depth, ostream& out) const {
	//out << "weak front rate: " << _weak_front.top_rate() << endl;
	//out << "strong front rate: " << _strong_front.top_rate() << endl;
	_weak_front.liquidity(depth,out);
	_strong_front.liquidity(depth,out);

}

c::info::info(const vector<efp>& ordered_ruler, front::info&& w, front::info&& s, efp center, int width): weak(move(w)), strong(move(s)) {
	auto i=std::lower_bound(ordered_ruler.begin(),ordered_ruler.end(),center);
	auto j=i;
	i-=width/2;
	j+=width/2;
	if (distance(ordered_ruler.begin(),i)<0) {
		i=ordered_ruler.begin();
		j=i+width;
	}
	else if (distance(ordered_ruler.end(),j)>=0) {
		j=ordered_ruler.end();
		i=j-width;
	}
	for (auto h=i; h!=j; ++h) {
		emplace(*h,vt(front::info::value_type(),false));
	}
	for (auto& h:weak) {
		find(h.first)->second=vt(h.second,false);
	}
	for (auto& h:strong) {
		find(h.first)->second=vt(h.second,false);
	}

	//pair<efp,efp> g=gap_encoded();
	begin()->second.second=true;
	rbegin()->second.second=true;
}

c::info c::get_info(efp center, int width) const {
	return move(info(_valid_rates_sorted, _weak_front.get_info(), _strong_front.get_info(), center, width));
}
pair<double,double> c::info::gap() const {
	auto g=gap_encoded();
	return pair<double,double>(decode_dbl(g.first),decode_dbl(g.second));
}
pair<double,double> c::info::range() const {
	auto i=begin();
	i++;
	efp tick=i->first-begin()->first;
	return pair<double,double>(decode_dbl(begin()->first),decode_dbl(rbegin()->first+tick));
}

pair<efp,efp> c::info::gap_encoded() const {
	pair<efp,efp> g;
	auto i=begin();
	i++;
	efp tick=i->first-begin()->first;
	if (weak.empty()) {
		g.first=begin()->first;
	}
	else {
		g.first=weak.rbegin()->first+tick;
	}
	if (strong.empty()) {
		g.second=rbegin()->first+tick;
	}
	else {
		g.second=strong.begin()->first;
	}
	return move(g);
}

void c::info::write(std::ostream& os) const {
	auto g=gap_encoded();
	efp gm=g.first+(g.second-g.first)/2;
	for (auto& i:*this) {
		os << decode_std(i.first);
		os << " \"";
		if (i.second.second)  os << decode_std(i.first);
		os << "\" ";
		os << (i.first<gm?weak.cur:strong.cur) << " ";
		i.second.first.write(os);
	}
}


void c::valid_rates_columns(ostream& os) {
	os << "<rate> <currency name>/<currency name>" << endl;
}

void c::valid_rates(ostream& os) const {
	auto w=curstr(_weak_currency);
	auto s=curstr(_strong_currency);
	for (const auto& r:_valid_rates_sorted) {
		os << decode(r) << L" " << w << L"/" << s << endl;
	}
}

c::spots c::valid_rates() const {
	return move(spots(_weak_currency,_strong_currency, _valid_rates_sorted));
}
void c::valid_rates_simple_columns(ostream& os) {
	os << "<rate>" << endl;
}

void c::valid_rates_simple(ostream& os) const {
	for (const auto& r:_valid_rates_sorted) {
		os << decode(r) << endl;
	}
}


c::spots::spots(spots&& other):base(move(other)),weak_cur(other.weak_cur),strong_cur(other.strong_cur) {
}
//void c::spots::sort() {
//	std::sort(begin(),end());
//}
void c::spots::dump(ostream& os) const {
	for (auto i:*this) {
		os << decode(i) << endl;
	}
	os << curstr(weak_cur) << "/" << curstr(strong_cur) << endl;
}
#include <algorithm>
double c::spots::lower(const double& ref) const {
	auto i=lower_rate(ref);
	return i!=end()?decode_dbl(*i):0;
}
double c::spots::upper(const double& ref) const {
	auto i=upper_rate(encode(ref));
	return i!=end()?decode_dbl(*i):0;
}
c::spots::const_iterator c::spots::lower_rate(const double& ref) const {
	auto v=encode(ref);
	auto i=lower_bound(begin(),end(),v);
	if (i!=end()) if (*i>=v) --i;
	return i;
}
c::spots::const_iterator c::spots::upper_rate(const double& ref) const {
	return upper_bound(begin(),end(),encode(ref));
}
double c::spots::price(const_iterator i) const {
        return decode_dbl(*i);
}

