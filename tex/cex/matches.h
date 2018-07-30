#ifndef CUREX_CEX_MATCHES_H
#define CUREX_CEX_MATCHES_H

#include "cex.h"
#include <vector>
#include <unordered_map>

namespace curex {
namespace cex {

struct matchitem {
	struct info {
		efp _cost;
		int _cost_cur;
		efp _received;
		int _received_cur;
		bool _buy;
		bool _weak;
		void dump(const string& prefix, ostream&os) const;
	};

	matchitem() {}
	id _lay_order_id;
	info _lay;
	info _back;
	efp _rate;
	void dump(const string& prefix, ostream&os) const;
};
class order;

struct matches:std::vector<matchitem> {
	efp unmatched{0};
	efp cost_rounding{0};
	vector<order*> _expired_orders;
	vector<id> _updated_orders;
	void dump(const string& prefix, ostream&os) const;
	//void report(int curid, int recvcurid, int weakcur, int strongcur, ostream& os) const;

};



}
}
#endif

