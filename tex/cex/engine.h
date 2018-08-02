#ifndef CUREX_CEX_ENGINE_H
#define CUREX_CEX_ENGINE_H

#include "cex.h"
#include "account.h"
#include "market.h"
#include "front.h"
#include "markets.h"
#include <mutex>
#include <vector>


namespace curex { namespace cex {


class engine {
public:
	struct env {
		struct nid {
			nid(const std::string& homebase, const std::string& subdir, const std::string& filename);
			id get();
			void load();
			void save();

			inline bool persistence() const { return persist; }
			bool persist;
			id _next_id{1};
			std::mutex _mx;
			std::string home;
			std::string file;
		};

		bool persist;
		env(std::string home_);
		std::string home;
		nid nid_account;
		nid nid_order;
	};
	//mutable std::mutex _mx;
	//static bool initialized;
	inline bool persistence() const { return _env.persist; }

	typedef std::unordered_map<id,account*> accounts;
	mutable accounts _accounts;
	mutable std::mutex _mx_accounts;


	//static void init();

	typedef std::unordered_map<market::key,market*,market::key_hasher> markets;
	markets _markets;  //it is constant after construction, no mutex

	account* _system_account;
	const id& get_system_account_id() const { return _system_account->_id; }

	void check_integrity() const;

	
	engine(std::string home, ostream& os);
	~engine();

	void list_currencies(ostream& os) const;

	//void dump(const string& prefix, ostream& os) const;
	//void dump_accounts(const string& prefix, ostream& os) const;
	void dump_markets(const string& prefix, ostream& os) const;

	struct stats {
		struct t : std::unordered_map<int,double> {
			void add(int cur, double v);
			t operator+(const t& other) const;
			void dump(ostream& os) const;
			void dump_line(std::ostream& os, int cur) const;
		};
		void dump(ostream& os) const;
		void dump_line(std::ostream& os, int cur) const;
		t money_in_exchange;
		t money_in_orders;
		t money_in_accounts;
		t money_in_system_account;
		
	};

	stats get_stats() const;


private:
//	typedef account::orderdata orderdata;
	typedef account::orders orders;
	orders _orders;
	//mutable std::mutex _mx_orders;
 
public:
	static void markets_columns(ostream&os);
	static void valid_rates_columns(ostream&os);
	static void valid_rates_simple_columns(ostream&os);
	static void trades_columns(ostream&os); 
	static void unmatches_columns(ostream&os);
	static void account_columns(ostream&os);
	static void liquidity_columns(ostream&os);
//	static void liquidity_simple_columns(ostream&os);

	bool check_withdrawn(const efp& withdrawn, const efp& requested, ostream&os) const;

public:
	struct dorder {
		//U 4 B 100.0000 GBP 2.0000 AUD/GBP
		char_type key; //U unmatched; M matched
		id _id;
		char_type buy;
		string amount;
		string cur;
		string rate;
		string market;
		int get_cid() const;
		efp get_encoded_rate() const;
		double get_rate() const;
		double get_amount() const;
		void dump(ostream& os) const;
	};
	struct dorders: std::vector<dorder> {
		dorders() {}
		dorders(dorders&&other):std::vector<dorder>(move(other)) {}
		dorders(const dorders&other):std::vector<dorder>(other) {}
		void dump(ostream& os) const;
		void filter_market(const string& market);
		void filter_front(int cid);
		dorders filter_market(const string& market) const;
		dorders filter_front(int cid) const;
	};
	dorders get_orders(const id& acid, const string&pwd) const;

	struct dtrade {
		//M 121 B 80.00000 AUD 1.38995 AUD/USD 57.55600 USD 8.00000 AUD
		id order_id;
		char_type buy;
		string amount;
		string cur;
		string rate;
		string market;
		string amount2;
		string cur2;
		string comm;
		string cur_comm;
		int get_cid() const;
		int get_cid2() const;
		int get_cid_comm() const;
		efp get_encoded_rate() const;
		double get_rate() const;
		double get_amount() const;
		double get_amount2() const;
		double get_commission() const;
		void dump(ostream& os) const;
	};
	struct dtrades: std::vector<dtrade> {
		dtrades() {}
		dtrades(dtrades&&other):std::vector<dtrade>(move(other)) {}
		dtrades(const dtrades&other):std::vector<dtrade>(other) {}
		void dump(ostream& os) const;
		void filter_market(const string& market);
		void filter_front(int cid);
		dtrades filter_market(const string& market) const;
		dtrades filter_front(int cid) const;
	};
	dtrades get_trades(const id& acid, const string&pwd) const;

	struct dliqitem {
		//1.36960 60.00000 1
		string rate;
		string amount;
		string cur;
		int get_cid() const;
		efp get_encoded_rate() const;
		double get_rate() const;
		double get_amount() const;
		void dump(ostream& os) const;
	};
	struct dliq: std::vector<dliqitem> {
		dliq() {}
		dliq(dliq&&other):std::vector<dliqitem>(move(other)) {}
		dliq(const dliq&other):std::vector<dliqitem>(other) {}
		void dump(ostream& os) const;
		void filter_front(int cid);
		dliq filter_front(int cid) const;
	};
	dliq get_liquidity(int cur1, int cur2, unsigned int depth) const;

	typedef account::dwallets dwallets;
	dwallets get_wallets(const id& acid, const string&pwd, ostream& os) const;
	double get_wallet(const id& acid, const string&pwd, int cid, ostream& os) const;

	market& get_market(int cur1, int cur2);

	void total_money(ostream& os) const;
	void total_money(std::unordered_map<int,efp>& g, ostream& os) const;

#ifdef MONEY_LEAK_TEST
void reset_check_money();
void check_money() const;

mutable int num_checks=-1;
mutable std::unordered_map<int,efp> _g;
#endif

	void auto_match_is_not_free() {
		auto_match_free=false;
	}

	bool auto_match_free{true};

	typedef market::info info;
	info get_info(int cid1, int cid2, efp center, int width) const;

public:
	//void set_ref_rate(int cur, int cur2, const double& ref);
	void set_ruler(int cur, int cur2, const double& min,const double& max,const double& increment);
	void set_ruler(int cur, int cur2, const double& min,const double& max);

	id new_account(const string&pwd, ostream& os);
	void new_wallet(const id& acid, const string&pwd, int cid, ostream& os);
	void delete_wallet(const id& acid, const string&pwd, int cid, ostream& os);
	void delete_all_wallets(const id& acid, const string&pwd, ostream& os);
	void deposit(const id& acid, const string&pwd, int cid, const string& amount, ostream& os);
	void deposit(const id& acid, const string&pwd, int cid, const double& amount, ostream& os);
	void list_account(const id& acid, const string&pwd, ostream& os) const;
	void status(const id& acid, const string&pwd, ostream& os) const;
	void status_sys_account(ostream& os) const;
	void _report_status(const account&, ostream& os) const;
	void list_orders(const id& acid, const string&pwd, ostream& os) const;
	void list_trades(const id& acid, const string&pwd, ostream& os) const;
	void liquidity(int cid1, int cid2, unsigned int depth, ostream& os) const;
	void liquidity(int cid1, int cid2, ostream& os) const { liquidity(cid1,cid2,-1,os); }
//	void liquidity_simple(int cid1, int cid2, unsigned int depth, ostream& os) const;
//	void liquidity_simple(int cid1, int cid2, ostream& os) const { liquidity_simple(cid1,cid2,-1,os); }
	void valid_rates(int cid1, int cid2, ostream& os) const;
	market::spots valid_rates(int cid1, int cid2) const;
	void valid_rates_simple(int cid1, int cid2, ostream& os) const;
	void list_markets(ostream& out) const;
	void list_market(int cid1, int cid2, ostream& out) const;
	void withdraw(const id& acid, const string&pwd, int cid, const string& amount, ostream& os);
//	id trade(const id& acid, const string&pwd, const id& cid, const string& amount, const string& limit_rate, int target_cid, ostream&os);
	struct tradeinput {
		string id_of_client;
		string code;
		string amount;
		string cur;
		string rate;
		string rateunits;
		void set_amount(const double&);
		void set_cur(int);
		void set_rate(const efp&);
		void trace(ostream& os) const;
	};
	string get_market_str(int weakcur, int strongcur) const;
	void trade(const id& acid, const string&pwd, const std::vector<tradeinput>& input, ostream&os);
	void trade_low(const id& acid, const string&pwd, const std::vector<tradeinput>& input, ostream&os);

	struct updateinput {
		string order_id;
		string amount;
		string rate;
		void set_order_id(const id&);
		void set_amount(const double&);
		void set_rate(const efp&);
		void trace(ostream& os) const;
	};
	void update(const id& acid, const string&pwd, const std::vector<updateinput>& input, ostream&os);

	void cancel_order(const id& acid, const string&pwd, const id& order_id, ostream&os);
	id update_order(const id& acid, const string&pwd, const id& order_id, const string& amount, ostream&os);
	void update_order(account& acc, const updateinput& input, ostream& os);
	void cancel_all_orders(const id& acid, const string&pwd, ostream&os);

	void set_commission_discount(const id& acid, const double&);
//int nid{0}; //para borrar, mirar constructor
private:
	account* get_account(const id& acid) const;
	account* auth(const id& acid, const string&pwd, ostream& os) const;

	id _sell(account& acc, market&, int cid, efp limit_rate, const efp& amount, bool buy, bool pri_low, ostream& os);
	void _trade(account& acc, const tradeinput& input, bool pri_low, ostream&os);

	const order* _update_order(account& acc, order& o, const efp& amount, ostream&os);
	void new_wallet(const id& acid, int cid);
	bool _cancel_order(account& acc, order& o, ostream& os);

	void track_orders(market& m);
	void create_markets();
	void track_orders();

public:
	void add_sample_rulers(ostream&os);
	void add_sample_liquidity(ostream&os);
private:
	void add_sample_liquidity2(id acid,market& m, int cur, const front::rates& rts, efp mean, bool weak, efp amount, ostream&os);
	void add_sample_liquidity(id acid,market& m, int cur, const front::rates& rts, efp mean, bool weak, ostream&os);
	void add_sample_liquidity(id acid,market& m, const double& refrate, ostream&os);

public:
	env _env;
	string system_acc_pwd;
	
private:
//	void dump_orders(const string& prefix, ostream& os) const;

	id process(account& acc, const market::result& r, ostream& os);
	void process(account& acc, const matches& m, int strong_cur, ostream& os);
	efp pay(account& acc, int cur, const efp& amount); //returns commission paid
	void pay0(account& acc, int cur, const efp& net, const efp& commission);
};




}}




#endif

