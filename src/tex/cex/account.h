#ifndef CUREX_CEX_ACCOUNT_H
#define CUREX_CEX_ACCOUNT_H
#include "cex.h"
#include <unordered_map>
#include "market.h"
#include "wallet.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <mutex>
#include <map>

namespace curex { namespace cex {
class order;
class engine;
class account: public std::unordered_map<int,wallet>  { //currency id
	typedef std::unordered_map<int,wallet> B;
public:
	engine& e;
	mutable std::mutex _mx;
	B::iterator find(int);
	B::const_iterator find(int) const;

	static bool exists(const engine&e_,const id& id_);
	account(engine&, const id& id_); //load
	account(engine&, const string& pwd); //save
	bool new_wallet(int cid);
	bool delete_wallet(int cid);
	bool delete_all_wallets();
	void deposit(int cid, const efp& amount);
	efp withdraw(int cid, const efp& amount); //returns amount withdrawn
	//void dump(const string& prefix, ostream& os) const;
	bool check_password(const string& pwd) const;

private:
	bool _new_wallet(int cid);
public:
	struct dwallets: std::map<int,double> {
	};
	dwallets get_wallets() const;
	double get_wallet(int cid) const;

	struct tradedata {
		tradedata(
			id order_id,
			efp rate, int weak_curid, int strong_curid,
			int paid_curid, efp paid_amount,
			int received_curid, efp received_amount,
			int commission_curid, efp commission_amount,
			bool weak,bool buy,bool lay);



		id _trade_id;
		id _order_id;
		efp _rate;
		int _weak_curid;
		int _strong_curid;

		int _paid_curid;
		efp _paid_amount;
		int _received_curid;
		efp _received_amount;
		int _commission_curid;
		efp _commission_amount;
		bool _lay;
		bool _buy;
		bool _weak;

		void report(ostream& os) const;
		static void report_columns(ostream& os);

		private:
		static id _next_id;
		static std::mutex _mx_next_id;

	};

	struct orders: std::unordered_map<id,order*> {
		typedef std::unordered_map<id,order*> B;
		mutable std::mutex _mx;
		B::const_iterator find(const id& id_) const;
		B::iterator find(const id& id_);
		void erase(const id& id_);
		void emplace(const id& id_,order*);

		B::const_iterator _find(const id& id_) const;
		B::iterator _find(const id& id_);
		void _erase(const id& id_);
		void _emplace(const id& id_,order*);
	};
	orders _orders;

	struct transaction: private std::vector<tradedata> {
		transaction();
		id _transaction_id;

		efp _rate;
		int _weak_curid;
		int _strong_curid;

		int _paid_curid;
		efp _paid_amount;
		int _received_curid;
		efp _received_amount;
		int _commission_curid;
		efp _commission_amount;

		void register_trade(tradedata&& td);
		void report(ostream& os) const;
		void recalculate();

		static void report_columns(ostream& os);

		private:
		static id _next_id;
		static std::mutex _mx_next_id;


	};


	struct transactions: std::vector<transaction> {
		mutable std::mutex _mx;
		void report(ostream& os) const;
		void register_transaction(transaction&& tr);

	};
	transactions _transactions;
	

	void untrack_order(const id& order_id);
	void track_order(order&);


	void register_trade(tradedata&& td);
	void register_transaction(transaction&& tr);

	static void list_columns(ostream& os);
	void list(ostream& os) const;
	void list_orders(ostream& os) const;
	void list_trades(ostream& os) const;

	void set_commission_discount(const double&d);

	id _id{0};
	size_t _password;
	double _commission_discount;

	static std::string get_home(const engine&e_,const id&);
	static std::string data_file(const engine&e_,const id&);
	
	std::string data_file() const;
	std::string wallets_file() const;
private:
	size_t hash(const string& s) const;

	std::string home;
public:
	void save_wallets() const;
private:
	void _save_wallets() const; //assumes _mx is locked
	void _load_wallets();
	void _load_data();
	void _save_data() const;
public:

/*
	//static bool init();
	std::string next_id_file();
	void load_next_id();
	void save_next_id();
*/
	//static id _next_id;
	//static std::mutex _mx_next_id;
};


}}

#endif

