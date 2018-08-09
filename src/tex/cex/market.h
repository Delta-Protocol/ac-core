#ifndef CUREX_CEX_MARKET_H
#define CUREX_CEX_MARKET_H
#include "cex.h"
#include "front.h"
#include <utility>
#include <vector>
#include <unordered_set>
#include <set>

namespace curex { namespace cex {

class wallet;
class engine;

class market {
public:
	market(engine&,id weakcur,id strongcur);
	
	int  _weak_currency{0};
	int  _strong_currency{0};

	int other_cur(int cur) const { return cur==_weak_currency?_strong_currency:_weak_currency; }

public:
	struct ruler_def {
		efp _ref{0};
		efp _min{0};
		efp _max{0};
		efp _increment{rate_multiples};
		void update(const efp& min,const efp& max,const efp& increment=rate_multiples);
		front::rates generate_rates() const;
		efp snap(efp v) const;
		
	};
	ruler_def _ruler_def;
	//efp _ref_rate;

	engine& e;
public:
	static void market_columns(ostream&);
	
public:
	//void set_ref_rate(const efp& value);
	//const efp& get_ref_rate() const;


	void set_ruler(const efp& min,const efp& max,const efp& increment);
	void set_ruler(const efp& min,const efp& max);

	front _weak_front;
	front _strong_front;

	struct key:std::pair<int,int> { 
		key():pair<int,int> (0,0) {
		}
		key(int i1, int i2):pair<int,int> (i1<i2?i1:i2,i1<i2?i2:i1) { ///has the same hash regardles the order
		}
		bool operator==(const key& other) const {
			return first == other.first && second == other.second;
		}
	};

	struct key_hasher {
	  std::size_t operator()(const key& k) const {
	    using std::size_t;
	    using std::hash;
	    return hash<int>()(k.first) ^ ((hash<int>()(k.second) << 1));
	  }
	};

	key get_key() const { return key(_weak_currency,_strong_currency); }

	void get_orders(std::vector<order*>& ords) const;

	void liquidity(unsigned int depth, ostream& out) const;

//	void liquidity_simple(unsigned int depth, ostream& out) const;
	struct info: std::map<efp,std::pair<front::info::value_type,bool>> {
		typedef std::pair<front::info::value_type,bool> vt;
		info() {}
		info(const std::vector<efp>& ordered_ruler, front::info&&, front::info&&, efp center, int width);
		
		front::info weak;
		front::info strong;
		void write(std::ostream& f) const;
		std::pair<efp,efp> gap_encoded() const;
		std::pair<double,double> gap() const;
		std::pair<double,double> range() const;

	};
	info get_info(efp center, int width) const;



	front::rates _valid_rates;
	std::vector<efp> _valid_rates_sorted;

	void valid_rates(ostream& os) const;

	struct spots:std::vector<efp> {
		typedef std::vector<efp> base;
		spots(): weak_cur(0), strong_cur(0) {}
		spots(int w,int s, const std::vector<efp>& r):weak_cur(w), strong_cur(s), base(r) {}
		spots(const spots&);
		spots(spots&&);
		//void sort();
		double lower(const double& ref) const;
		double upper(const double& ref) const;
		const_iterator lower_rate(const double& ref) const;
		const_iterator upper_rate(const double& ref) const;
		double price(const_iterator i) const;
		int weak_cur;
		int strong_cur;
		void dump(ostream& os) const;
	};

	spots valid_rates() const;
	void valid_rates_simple(ostream& os) const;

	static void valid_rates_columns(ostream&);
	static void valid_rates_simple_columns(ostream&);



	struct result {
		bool error{false};
		matches _matches;
		order* _order{0}; //unmatched
		//efp _refund{0};
		int _paid_cur{0};
		int _recv_cur{0};
		efp _order_rate; //weak rate

		//efp _ref_rate{0}; //weak ref rate
		bool buy;
		int _weak_cur{0};
		int _strong_cur{0};

		void dump(const string& prefix, ostream& os) const;
		//void report(ostream& os) const;
	};
//	double to_nominal_rate(id cur, const double& currate) const;
//	double from_nominal_rate(id cur, const double& currate) const;

	result trade_weak(wallet& acc, const efp& limit_rate, const efp& amount, bool buy, bool pri_low, ostream& os);
	result trade_strong(wallet& acc, const efp& limit_rate, const efp& amount, bool buy, bool pri_low, ostream& os);
	result trade(wallet& acc, id cur, const efp& limit_rate, const efp& amount, bool buy, bool pri_low, ostream& os);
	order* update_order(const id& order_id, const efp& amount, ostream& os);
	order* cancel_order(const id& order_id, ostream& os);
	string get_name() const;

	void dump(const string& prefix, ostream& os) const;

};





}}

#endif

