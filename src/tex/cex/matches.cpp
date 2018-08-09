#include "matches.h"
#include <iostream>

using namespace curex::cex;
using namespace std;

typedef curex::cex::matches c;
/*
c::summary c::_get_summary() const {
	summary s;
	if (empty()) return move(s);
//	int n=0;
	for (auto i:*this) {
		s.cost+=i._cost;
		s.received+=i._received;
//		++n;
//		s.avgrate+=i._rate;
	}
//	double avgrt=decode_dbl(s.avgrate);
//	avgrt/=(double)n;
//	s.avgrate=encode(avgrt);
	s.realrate=encode(decode_dbl(s.received)/decode_dbl(s.cost));
	s.cost+=cost_rounding;
	return move(s);
}
*/

void matchitem::info::dump(const string& prefix, ostream&os) const {
	os << prefix << L" cost " << decode(_cost) << L" " << curstr(_cost_cur) <<
		L" received " << decode(_received) << L" " << curstr(_received_cur) <<
		L" " << (_buy?L"buy ":L"sell ") << (_weak?L"weak":L"strong") << endl;
}

void matchitem::dump(const string& prefix, ostream&os) const {
	os << prefix << L"lay order " << _lay_order_id << L" rate " << decode(_rate) << std::endl;
	_lay.dump(prefix+L"   lay>",os);
	_back.dump(prefix+L"  back>",os);
}

#include "order.h"

void c::dump(const string& prefix, ostream&os) const {
	for (auto i:*this) {
		i.dump(prefix+L"  ",os);
	}
//	_summary.dump(prefix+L"  ",os);
	os << prefix << L"remaining unmatched " << decode(unmatched) << L" rounding cost " << decode(cost_rounding) << endl;
	if (!_expired_orders.empty()) {
		os << prefix << L"expired orders: ";
		for (auto i:_expired_orders) os << prefix << L"  " << i->_id << L" ";
		os << endl;
	}

}
/*
void c::report(int curid, int recvcurid, int weakcur, int strongcur, ostream& os) const {
//	dump(L">>> ",wcout);
	_summary.report(curid,recvcurid,weakcur, strongcur, os);
}
*/
/*
void c::summary::dump(const string& prefix, ostream&os) const {
	os << prefix << L"summary: cost " << decode(cost) << L" received " << decode(received) << L" real rate " << decode(realrate) << endl;
}
void c::summary::report_columns(ostream& os) {
	os << L"M <amount paid> <currency> <rate> <rate units> <amount received> < currency>" << endl;
}
void c::summary::report(int curid, int recvcurid, int weakcur, int strongcur, ostream& os) const {
	if (cost==0) return;
	os << L"M " << decode(cost) << L" "  << curstr(curid) << L" " << decode(realrate) << " " << curstr(weakcur) << "/" << curstr(strongcur)  << " " << decode(received) << L" " << curstr(recvcurid) << endl;
}
*/




