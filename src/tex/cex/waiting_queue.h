#ifndef CUREX_CEX_WAITING_QUEUE_H
#define CUREX_CEX_WAITING_QUEUE_H

#include "cex.h"
#include "rate.h"
#include "matches.h"
#include <unordered_map>
#include <unordered_set>
#include <list>
#include <utility>
#include <vector>
#include <mutex>


namespace curex {
namespace cex {

class order;
class wallet;
class engine;
class waiting_queue;



class waiting_queue:public std::list<order*> { //doubly linked list, agile to pop, push, and withdraw from the middle
	typedef list<order*> B;

	mutable std::mutex _mx;

	typedef std::unordered_map<id,iterator> hash; //fastest to access by order id
	hash _hash;
	mutable std::mutex _mx_hash;
	
public:
	std::string home;

private:
	efp _value{0};
	mutable std::mutex _mx_value;
public:
	//bool empty() const;

	struct info {
		int size;
		efp value;
		void write(std::ostream& os) const;
	};

	info get_info() const;

	const efp& get_value() const;

	waiting_queue(engine&e,const efp& r, bool invert_priority, const std::string& parentdir);
	~waiting_queue();
	void dump(const string& prefix, ostream&os) const;

	order* new_order(const efp& amount, bool weak, bool buy, ostream& os);

	void get_orders(std::vector<order*>& ords) const;

	engine&e;
	order* cancel_order(const id& id_, bool save, ostream& os);
	order* update_order(const id& id_, const efp& amount, ostream& os);
	std::pair<bool,efp> match(wallet& w, matches& m, const efp& amount,int cur, int lay_cur, bool weak, bool buy, ostream& os);

	bool compare(const efp& otherrate) const;
//	bool operator < (const waiting_queue& other) const;
//	bool operator == (const waiting_queue& other) const;
	bool _invert_priority;

	void save() const;
	void load();
public:
	efp _rate;
private:
	order* _cancel_order(const id& id_, bool save, ostream& os);
	void _save() const;
};


struct waiting_queues {
	waiting_queues(engine&e,const efp& r, bool invert_priority, const std::string& parentdir);
	~waiting_queues();

	typedef std::unordered_set<id> hash; //fastest to access by order id
	hash _hash_low;
	mutable std::mutex _mx_hash_low;

        waiting_queue* high;
        waiting_queue* low;

	const efp& get_rate() const;

	struct info {
		waiting_queue::info high;
		waiting_queue::info low;
		void write(std::ostream& os) const;
	};

	info get_info() const;

	efp get_value() const;
	bool has_value() const;

	void get_orders(std::vector<order*>& ords) const;
	void dump(const string& prefix, ostream&os) const;
	order* new_order_low(const efp& amount, bool weak, bool buy, ostream& os);
	order* new_order(const efp& amount, bool weak, bool buy, ostream& os);
	order* cancel_order(const id& id_, bool save, ostream& os);
	order* update_order(const id& id_, const efp& amount, ostream& os);
	std::pair<bool,efp> match(wallet& w, matches& m, const efp& amount,int cur, int lay_cur, bool weak, bool buy, ostream& os);
	void save() const;
	void load();

};


}
}
#endif

