#ifndef CUREX_CEX_ORDER_H
#define CUREX_CEX_ORDER_H

#include "cex.h"
#include "rate.h"
#include "matches.h"
#include <utility>


namespace curex {
namespace cex {

class wallet;
class waiting_queue;
class engine;
class order {

public:
	engine& e;
	id _id;
	efp _value;
	bool _buy;
	bool _weak;
	explicit order(engine&,const efp&, bool weak, bool buy, waiting_queue& q);
	explicit order(engine&,const std::string&, waiting_queue& q);

	waiting_queue& _queue;

	efp budget_for_update(const efp& amount) const;

	order* update(const efp& amount);
	std::pair<bool,efp> match(wallet& w, matches& ms, const efp& rate, const efp& amount, int cur, int lay_cur, bool weak, bool buy, ostream&os);
	static bool could_match_provided_big_ordervalue(wallet& w, const efp& rate, const efp& amount, bool weak, bool buy,ostream& os);
	static bool could_match_provided_big_amount(const efp& rate, const efp& order_value, bool weak, bool buy);

	void write(std::ostream& os) const;

	void dump(const string& prefix, ostream& os) const;
	static void report_columns(ostream& os);
	void report(ostream& os) const;

	std::pair<int,efp> get_value() const;
	// order data
	//market::key _market_key;
	//bool buy;
	id _account_id;
	int _received_cur;
	int _paid_cur;
	efp _rate;
	int _weak_cur;
	int _strong_cur;
	void set_data(const id& acid, int received_cur, int paid_cur, const efp& rt, int weak_cur, int strong_cur);

	
/*
	static bool init();
	static std::string next_id_file();
	static void load_next_id();
	static void save_next_id();
*/
//	static id _next_id;
};



}
}
#endif

