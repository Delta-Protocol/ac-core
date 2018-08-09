#ifndef CUREX_CEX_FRONT_H
#define CUREX_CEX_FRONT_H

#include "cex.h"
#include "waiting_queue.h"
#include <queue>
#include <utility>
#include <vector>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <thread>


namespace curex {
namespace cex {

class order;
class wallet;
class engine;
struct compa {
	bool operator () (waiting_queues* q1, waiting_queues* q2) const{
		return q2->high->compare(q1->high->_rate);
	}
};

typedef std::chrono::time_point<std::chrono::high_resolution_clock> time;
using namespace std::chrono_literals;

template<typename T>
struct attic: private std::list<std::pair<time,T*>> {

	typedef std::list<std::pair<time,T*>> B;
	mutable std::mutex _mx;
	~attic() {
		for(auto i:*this) {
			delete i.second;
		}
	}
	void delete_later(T*t) {
		std::lock_guard<std::mutex> lock(_mx);
		flush();
		B::push_back(std::pair<time,T*>(std::chrono::high_resolution_clock::now(),t));
		
		
	}
private:
	void flush() {
		auto ref = std::chrono::high_resolution_clock::now()-std::chrono::seconds(2);
		for(auto i=B::begin(); i!=B::end(); ) {
			if (i->first<ref) {
				delete i->second;
				i=B::erase(i);  //erase(next(i).base());
			}
			else {
				++i;
			}
		}
	}
};

class front: public std::priority_queue<waiting_queues*,std::vector<waiting_queues*>,compa> { //O(1) access to top (matching)
	typedef waiting_queues Q;
	typedef std::vector<Q*> impl_t;
	typedef std::priority_queue<Q*,impl_t,compa> B;
	
	mutable std::mutex _mx;

	attic<Q> _attic;

	int _curid;
	bool _invert_priority;

	typedef std::unordered_map<efp,Q*> queues_by_rate;
	queues_by_rate _queues_by_rate;
	mutable std::mutex _queues_by_rate_mx;


	typedef std::unordered_map<id,Q*> queues_by_order; //fastest to access by order id
	queues_by_order _queues_by_order;
	mutable std::mutex _queues_by_order_mx;
public:
	typedef std::unordered_set<efp> rates;

	std::string home;
private:
	const rates* _valid_rates{0};
	std::mutex _valid_rates_mx;
public:
	void load();
	inline int get_cur() const { return _curid; };
	inline const queues_by_rate& get_queues_by_rate() const { return _queues_by_rate; }
public:
	front(engine&e,int curid, bool invert_priority);
	~front();
	engine&e;
	void set_rates(const rates& rs);

	void cancel_orders_not_in(const rates& rs);

	void get_orders(std::vector<order*>& ords) const;

	static void liquidity_columns(ostream&os);
	void liquidity(unsigned int depth, ostream& out) const;

	struct info: std::map<efp,Q::info> {
		typedef Q::info value_type;
		info(): cur(0) {}
		int cur;
		void write(std::ostream& os) const;
	};
	info get_info() const;

	bool has_order(const id& orderid) const;

	void dump(const string& prefix, ostream& os) const;

	order* new_order(const efp& rate, const efp& amount, bool weak, bool buy, ostream& os);
	order* new_order_low(const efp& rate, const efp& amount, bool weak, bool buy, ostream& os);

	order* cancel_order(const id& id_, ostream& os);
	order* update_order(const id& id_, const efp& amount, ostream& os);

	matches match(wallet& w, const efp& limit, const efp& amount, int cur, int lay_cur, bool buy, ostream& os);
	//bool could_match(const efp& resident_order_rate, const efp& amount) const;
	bool test_match(const efp& limit_rate) const;



private:
	Q* get_queue_rate(const efp& r, ostream& os);
	const Q* get_queue_rate(const efp& r) const;
	Q* get_queue_id(const id& id_);

	void trim();
};


}
}
#endif

