#include "order.h"
#include "wallet.h"
#include "engine.h"
#include <boost/filesystem.hpp>

using namespace curex::cex;
using namespace std;

typedef curex::cex::order c;
//id c::_next_id{1};


c::order(engine& e_,const efp& ev, bool weak, bool buy, waiting_queue& q):  _value(ev), _weak(weak), _buy(buy), _account_id(0), _received_cur(0), _paid_cur(0), _rate(0), _weak_cur(0), _strong_cur(0), _queue(q), e(e_) { 
	_id=e._env.nid_order.get();
}

c::order(engine& e_,const std::string& line, waiting_queue& q): _queue(q), e(e_) {
//cout << "parsing line " << line << endl;
	std::istringstream is(line.c_str());
	is >> _id;
	is >> _value;
	is >> _buy;
	is >> _weak;

	is >> _account_id;
	is >> _received_cur;
	is >> _paid_cur;
	is >> _rate;
	is >> _weak_cur;
	is >> _strong_cur;
//cout << "creo order id " << _id << " (load from " << line << ")" << endl;
//cout << "    rate value " << _rate << " " << _value << endl;
}



efp c::budget_for_update(const efp& amount) const {
	return amount-_value;
}

order* c::update(const efp& v) {
	if (v==_value) return 0;
	if (v<_value) {
		_value=v;
		return 0;
	}
	order* o=new order(e,v-_value,_weak,_buy,_queue);
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
//
bool c::could_match_provided_big_ordervalue(wallet& wallet, const efp& rate, const efp& amount, bool weak,bool buy,ostream& os) {
//weak front:
	//buy true, EUR; 
	//buy false GBP; 
	efp ev_left_to_match;
	if (buy) {
		ev_left_to_match=amount;
	}
	else {
		if (weak) {
			ev_left_to_match=encode(decode_dbl(amount)*decode_dbl(rate));	
		}
		else {
			ev_left_to_match=encode(decode_dbl(amount)/decode_dbl(rate));	
		}
	}
	if (ev_left_to_match==0) {
		return 0;
	}
	efp received;
	efp cost;
	if (buy) {
		cost=ev_left_to_match;
		received=encode(decode_dbl(cost)/decode_dbl(rate));
	}
	else {
		received=ev_left_to_match;
		cost=encode(decode_dbl(received)/decode_dbl(rate));
	}
	if (cost==0) {
		return 0;			
	}
	if (wallet.available<cost) {
		if (wallet.available==0) {
			os << "Error: E301; No funds in wallet." << endl;
		}
		else {
			os << "Error: E11; Not enough funds in wallet yo achieve the amount requested. All available amount is used instead (" << decode(wallet.available) << ")." << endl;
		}
		//calculate the trade to leave the wallet empty
		cost=wallet.available;
		efp newamount;
		if (buy) {
			if (weak) {
				newamount=encode(decode_dbl(cost)*decode_dbl(rate));
			}
			else {
				newamount=encode(decode_dbl(cost)/decode_dbl(rate));
			}
		}
		else {
			newamount=cost;
		}
		return could_match_provided_big_ordervalue(wallet, rate, newamount, weak,buy,os);
	}
	return true;
}
bool c::could_match_provided_big_amount(const efp& rate, const efp& order_value, bool weak,bool buy) {
//cout << "decode_dbl(order_value): " << decode_dbl(order_value) << endl;
//cout << "decode_dbl(rate): " << decode_dbl(rate) << endl;
	efp cost;
	if (buy) {
		cost=order_value;
	}
	else {
		if (weak) {
			cost=encode(decode_dbl(order_value)/decode_dbl(rate));
		}
		else {
			cost=encode(decode_dbl(order_value)*decode_dbl(rate));
		}
	}

////cout << "cost: " << cost << endl;
	if (cost==0) {
		return false;			
	}
	return true;
}


pair<int,efp> c::get_value() const {
	return pair<int,efp>(_weak?_weak_cur:_strong_cur,_value);
}


//return codes
//true order has been updated
//false input amount is too small to match or not enough money in wallet


pair<bool,efp> c::match(wallet& wallet, matches& ms, const efp& rate, const efp& amount, int cur, int lay_cur, bool weak, bool buy, ostream& os) {
//os << "--order::match--" << endl;
	matchitem m;

/*
efp _cost;
int _cost_cur;
efp _received;
int _received_cur;
*/

	efp to_match; //same units as value
	if (buy) {
		to_match=amount;
//os << "     to_match " << decode(to_match) << endl;
	}
	else {
		if (weak) {
			to_match=encode(decode_dbl(amount)*decode_dbl(rate));	
		}
		else {
			to_match=encode(decode_dbl(amount)/decode_dbl(rate));	
		}
	}

	if (to_match==0) {
		return pair<bool,efp>(false,amount);
	}

	if (to_match>_value) {
		m._back._received=_value;
		if (weak) {
			m._back._cost=encode(decode_dbl(m._back._received)/decode_dbl(rate));
		}
		else {
			m._back._cost=encode(decode_dbl(m._back._received)*decode_dbl(rate));
		}
	}
	else {
		m._back._received=to_match;
		if (buy) {
			if (weak) {
				m._back._cost=encode(decode_dbl(m._back._received)/decode_dbl(rate));
			}
			else {
				m._back._cost=encode(decode_dbl(m._back._received)*decode_dbl(rate));
			}
		}
		else {
			m._back._cost=amount;
//			if (weak) {
//				m._cost=encode(decode_dbl(m._received)/decode_dbl(rate));
//				m._cost=amount;
//			}
//			else {
//				m._cost=encode(decode_dbl(m._received)*decode_dbl(rate));
//				m._cost=amount;
//			}
		}
	}
//os << "     COST " << decode(m._cost) << " RECEIVED " << decode(m._received) << endl;

	if (wallet.available<m._back._cost) {
		if (wallet.available==0) {
			os << "Error: E301; No funds in wallet." << endl;
		}
		else {
			os << "Error: E11; Not enough funds in wallet yo achieve the amount requested. All available amount is used instead (" << decode(wallet.available) << ")." << endl;
		}
		//calculate the trade to leave the wallet empty
		efp cost=wallet.available;
		efp amount;
		if (buy) {
			if (weak) {
				amount=encode(decode_dbl(cost)*decode_dbl(rate));
			}
			else {
				amount=encode(decode_dbl(cost)/decode_dbl(rate));
			}
		}
		else {
			amount=cost;
		}
		return move(match(wallet, ms, rate, amount, cur, lay_cur, weak, buy, os));
	}


	if (m._back._cost==0) {
//os << "B " << m._received << endl;
		return pair<bool,efp>(false,amount);			
	}

	wallet.withdraw(m._back._cost);

	if (to_match>_value) {
		_value=0;
	}
	else {
		_value-=m._back._received;
	}
//os << "     order value aftermatch " << decode(_value) << endl;
	if (_buy) {
		if (buy) { //ok
			m._back._cost_cur=lay_cur;
			m._back._received_cur=cur;
		}
		else {
			m._back._cost_cur=lay_cur;
			m._back._received_cur=cur;
		}
	}
	else {
		if (buy) {
			m._back._cost_cur=cur;
			m._back._received_cur=lay_cur;
		}
		else { //ok
			m._back._cost_cur=cur;
			m._back._received_cur=lay_cur;
		}
	}

	efp am=amount;
	if (buy) {
		am-=m._back._received;
	}
	else {
		am-=m._back._cost;
	}

	if (_buy) {
		swap(m._back._cost_cur,m._back._received_cur);
	}


	m._lay._cost=m._back._received;
	m._lay._cost_cur=m._back._received_cur;
	m._lay._received=m._back._cost;
	m._lay._received_cur=m._back._cost_cur;

	m._lay_order_id=_id;
	m._rate=rate;

	m._lay._buy=_buy;
	m._lay._weak=_weak;

	m._back._buy=buy;
	m._back._weak=weak;

	ms.push_back(m);
//if (_value<=0) os << "C" << endl;
	return pair<bool,efp>(true,am); //_value>0
}


void c::dump(const string& prefix, ostream& os) const {
	os << prefix << L" orid " << _id << L" value " << decode(_value) << endl;
}
void c::report_columns(ostream& os) {
	os << "U <order id> <B|S> <amount> <currency> <rate> <rate units>" << endl;
}

void c::report(ostream& os) const {
	int cur;
	efp v;
	if (_buy) {
		cur=_received_cur;
		if (_weak) {
			v=encode(decode_dbl(_value)/decode_dbl(_rate));
		}
		else {
			v=encode(decode_dbl(_value)*decode_dbl(_rate));
		}
	}
	else {
		cur=_paid_cur;
		v=_value;
	}
	os << L"U " << _id << L" " <<
	(_buy?L"B":L"S") << L" " <<
	decode(v) << L" " << curstr(cur) << " "
	<< decode(_rate) << " " << curstr(_weak_cur) << "/" << curstr(_strong_cur) << endl;
}

void c::write(std::ostream& os) const {
	os << _id << " " << _value << " " << (_buy?1:0) << " " << (_weak?1:0) << " " << _account_id << " " << _received_cur << " " << _paid_cur << " " << _rate << " " << _weak_cur << " " << _strong_cur << endl;
	
}

#include "waiting_queue.h"

void c::set_data(const id& acid, int received_cur, int paid_cur, const efp& rt, int weak_cur, int strong_cur) {
	_account_id=acid;
	_received_cur=received_cur;
	_paid_cur=paid_cur;
	_rate=rt;
	_weak_cur=weak_cur;
	_strong_cur=strong_cur;
	_queue.save();
}





