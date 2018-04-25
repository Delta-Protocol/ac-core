#ifndef USGOV_49e5fff0395fac385e48353d91ae777f7b0ba92baa74a883f127855ad412c9e0
#define USGOV_49e5fff0395fac385e48353d91ae777f7b0ba92baa74a883f127855ad412c9e0

#include <gov/auth.h>
#include <gov/blockchain.h>

#include <vector>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <gov/signal_handler.h>
#include <gov/crypto/crypto.h>
#include <gov/blockchain/diff.h>
#include "protocol.h"
#include <chrono>
#include <cassert>
#include <map>
#include <atomic>
#include "tx.h"


namespace usgov {
namespace cash {
	struct app_gut;
}}

namespace std {

  template <>
  struct hash<usgov::cash::app_gut> {
	size_t operator()(const usgov::cash::app_gut&) const;
  };



}

namespace usgov {
namespace cash {
	using namespace std;
	using socket::datagram;
	//using blockchain::signature;
	using blockchain::miner_gut;
	using crypto::ripemd160;
	using crypto::ec;
	using blockchain::peer_t;

	typedef int64_t cash_t;
	typedef crypto::ec::keys keys;
	typedef keys::pub_t pubkey_t;

	typedef ripemd160 hasher_t;
	typedef hasher_t::value_type hash_t;

//	typedef uint32_t spend_code_t;
//	static constexpr spend_code_t no_spend_code{0};
//	static constexpr spend_code_t min_spend_code{1};
	static const hash_t min_locking_program(10000);



	struct policies_traits {
		enum paramid { 
			minimum_fee=0,
			num_params,
		};
		constexpr static array<const char*,num_params> paramstr={"minimum_fee"/*,"lifestyle_minimum_level"*/};
	};

	struct app_gut: blockchain::policies_app_gut<double, policies_traits> {
		typedef blockchain::policies_app_gut<double, policies_traits> b;

		app_gut& operator =(int zero) {
			*this=app_gut();
			return *this;
		}

		app_gut() {
		}
		virtual ~app_gut() {
			//for (auto i:*this) delete i; // delete checkpoint_; 
		}
		virtual int app_id() const override;

		virtual void to_stream(ostream&) const override;
		virtual void from_stream(istream&) override;

		struct account_t {
			account_t() {}
			account_t(const hash_t& locking_program, const cash_t& balance): locking_program(locking_program), balance(balance) {}

			hash_t locking_program;
			cash_t balance;

			void dump(ostream& os) const;
			void to_stream(ostream& os) const {
				os << locking_program << ' ' << balance << ' ';
			}
			static account_t from_stream(istream& is) {
				account_t i;
				is >> i.locking_program;
				is >> i.balance;
				return move(i);
			}
		};

		typedef account_t batch_item;
		struct batch_t: unordered_map<hash_t,batch_item> {
			void add(const hash_t& address/*, const spend_code_t& spend_code*/, const batch_item& bi) {
				auto i=find(address);
				if (likely(i==end())) {
					emplace(address,bi);
				}
				else {
					i->second=bi;
				}
			}
		};

		struct accounts_t: map<hash_t,account_t> {
			void add(const batch_t& batch) {
				for (auto& b:batch) {
					auto i=find(b.first);
					if (likely(i==end())) {
						emplace(b);
					}	
					else {
						//i->second.spend_code=b.second.spend_code;
						i->second=b.second;
					}
				}
			}
			cash_t get_balance() const {
				cash_t b=0;
				for (auto&i:*this) {
					b+=i.second.balance;
				}
				return move(b);
			}
			void to_stream(ostream& os) const {
				os << size() << ' ';
				for (auto& i:*this) {
					os << i.first << ' ';
					i.second.to_stream(os);
				}
			}
			static accounts_t* from_stream(istream& is) {
				accounts_t* r=new accounts_t();
				size_t n;
				is >> n;
				for (size_t i=0; i<n; ++i) {
					hash_t h;
					is >> h;
					r->emplace(h,move(account_t::from_stream(is)));
				}	
				return r;
			}

			bool add_input(tx& t, const hash_t& addr, const cash_t& amount);
			bool add_output(tx& t, const hash_t& addr, const cash_t& amount, const hash_t& locking_program);
			
			void dump(ostream& os) const;
			bool pay(unsigned int seed, const hash_t& k, const cash_t& amount);
			bool withdraw(unsigned int seed, const hash_t& k, const cash_t& amount);
		};




		accounts_t accounts; 
		cash_t fees{0};

		mutable hash_t hash{0};
		const hash_t& get_hash() const {
			if (hash==0) hash=compute_hash();
			return hash;
		}

		hash_t compute_hash() const { //include only the elements controlled by majority_consensus
			hasher_t h;
			for (auto&i:accounts) {
				h.write(i.first);
				h.write(i.second.balance);
				h.write(i.second.locking_program);
			}
			h.write(fees);
			hash_t v;
			h.finalize(v);
			return move(v);
		}

	};

	struct app_gut2: blockchain::policies_app_gut2<double, policies_traits, blockchain::average_merger<double>> {
		typedef blockchain::policies_app_gut2<double, policies_traits, blockchain::average_merger<double>> b;
		typedef app_gut::accounts_t accounts_t;
		app_gut2() {}
/*
		 app_gut2(app_gut* g):b(*g) { //:policies(move(g->policies)) {
//			for (auto& i:g->to_hall) to_hall.emplace(i);
			delete g;
		}
*/
		virtual ~app_gut2() {
		}
		virtual uint64_t merge(blockchain::app_gut* other0) override {
			app_gut* other=static_cast<app_gut*>(other0);
			m.merge(*other,*other);		

			b::merge(other0);

cout << "MERGE: other.fees=" << other->fees << endl;
			return 0; //other->fees;
		}
		virtual void end_merge() override {
			m.end_merge(g, 0);
cout << "END MERGE: g.fees=" << g.fees << endl;

//			double m=multiplicity;
//			for (int i=0; i<policies_traits::num_params; ++i) policies[i]/=m;
		}
		virtual void to_stream(ostream& os) const override;
		static app_gut2* from_stream(istream& is);
//		map<pubkey,address> to_hall; //pubkey
//		array<double,policies_traits::num_params> policies;
		app_gut g;
		blockchain::majority_merger<app_gut> m;
		//cash_t fees{0};
	};

/*
	struct blockchain_app: blockchain::app { //this layer handles blockchain affairs (block construction) 
//		virtual bool in_service() const override;
		virtual void on_begin_cycle() override;

//		static unordered_map<const miner_gut*,uint64_t> to_fees(const unordered_map<const miner_gut*,double>& shares,uint64_t total_fees);
//		tx* pay_to_net(const blockchain::block& b); 
//		virtual blockchain::app_gut* create_closure_gut(const blockchain::block& b) override;

	};
*/
	struct app:blockchain::app { 
		app();
		virtual ~app();
		constexpr static const char* name={"cash"};
		virtual string get_name() const override { return name; }

		virtual void on_begin_cycle() override;

		static int id() { return 30; }
		virtual int get_id() const override { return id(); }

		virtual string shell_command(const string& cmdline) override;

	//	thread* verif_thread{0};
	//	virtual void on_head_ready() override; //can start verification

     //   	enum validation_result { valid, invalid, unable_to_validate };
		double supply_function(double x0, double x, double xf) const;

		virtual void run() override;
		void add_policies();

    //    validation_result validate(const tx& tr) const;
/*
		enum svcid {
			svc_tx_merchant=300,
			svc_tx_node=301,
		};
*/
		
		app_gut* pool{0};
		mutex mx_pool;

//        void incoming_transaction_from_node(peer_t *src, datagram*d);
//		void incoming_transaction_from_merchant(peer_t *src, datagram*d);
		struct query_accounts_t:vector<hash_t> {
			void add(const string& addr) {	
				istringstream is(addr);
				hash_t v;
				is >> v;
				emplace_back(move(v));
			}
			void add(const hash_t& v) {	
				emplace_back(v);
			}
			datagram* get_datagram() const {
				if (empty()) return 0;
				ostringstream os;
				os << size() << ' ';
				for (auto&i:*this) {	
					os << i << ' ';
				}
				return new socket::datagram(protocol::cash_query,os.str());
			}
			static query_accounts_t from_datagram(datagram*);
			static query_accounts_t from_string(const string&);
		};

		virtual blockchain::app_gut* create_app_gut() override;
		virtual bool process_work(peer_t *c, datagram*d) override;
		virtual bool process_query(peer_t *, datagram*) override;

		virtual bool process_evidence(peer_t *, datagram*) override;
		bool process_tx(peer_t *, datagram*);
		void cash_query(peer_t *, datagram*);



/*
		struct buffer_t:multimap<uint64_t,tx*> { //0 need verification, 1 verified; orderd by fee asc
			typedef multimap<uint64_t,tx*> b;
			~buffer_t();
			bool add(tx*t);
			void set_unverified();
			tx* next_unverified();
			void destroy(tx* t);
			app_gut* gut();
			unordered_set<uint64_t> index;
			mutex mx;
		};

		void verification_daemon();

		//bool exit;
		buffer_t buffer;
*/
		virtual void import(const blockchain::app_gut2&, const blockchain::pow_t&) override;
		//virtual void import(const blockchain::app_gut&);
	//	virtual void clear_db() override;
//		typedef double_sha256 hasher_t;


		typedef app_gut::account_t account_t;

		typedef app_gut::accounts_t accounts_t;


/*
		struct accounts_t: map<hash_t,account_t> {
			cash_t get_balance() const {
				cash_t b=0;
				for (auto&i:*this) {
					b+=i.second.balance;
				}
				return move(b);
			}
			void to_stream(ostream& os) const {
				os << size() << ' ';
				for (auto& i:*this) {
					os << i.first << ' ';
					i.second.to_stream(os);
				}
			}
			static accounts_t from_stream(istream& is) {
				accounts_t r;
				size_t n;
				is >> n;
				for (size_t i=0; i<n; ++i) {
					hash_t h;
					is >> h;
					account_t bi=account_t::from_stream(is);
					r.emplace(h,move(bi));
				}	
				return move(r);
			}

			bool add_input(tx& t, const hash_t& addr, const cash_t& amount);
			bool add_output(tx& t, const hash_t& addr, const cash_t& amount, const hash_t& locking_program);
			
			void dump(ostream& os) const;
			bool pay(unsigned int seed, const hash_t& k, const cash_t& amount);
			bool withdraw(unsigned int seed, const hash_t& k, const cash_t& amount);
		};
*/

		bool account_state(const app_gut::batch_t& batch, const hash_t& address, account_t& acc) const;

		bool checksig(const size_t& this_index, const string& locking_program_input, const tx&) const;
		static bool unlock(const hash_t& address, const size_t& this_index, const hash_t& locking_program, const string& locking_program_input, const tx&);

		bool process(const tx&);

		struct db_t {
			db_t() {
				accounts=new accounts_t();
			}
			db_t(db_t&& other):supply_left(other.supply_left), block_reward(other.block_reward) {
				accounts=other.accounts;
				other.accounts=0;
			}
			~db_t() {
				delete accounts;
			}
			//bool move(const string& src, const string& dst, uint64_t amount);
			//bool move(const tx::end_t& srcs, const tx::end_t& dsts);
			bool add_(unsigned int seed, const hash_t&, const cash_t& amount);
//			bool withdraw_(unsigned int seed, const spend_code_t& sc, const hash_t& k, const cash_t& amount, accounts_t::undo_t&);
			bool withdraw_(unsigned int seed, const hash_t& k, const cash_t& amount/*, accounts_t::undo_t&*/);

			void dump(ostream& os) const;
			//void clear();
			cash_t get_newcash();

			//typedef unordered_map<string,cash_t> accounts_t;
			void to_stream(ostream&) const;
			static db_t from_stream(istream&);

			mutable mutex mx;
			accounts_t* accounts;
			cash_t supply_left{2100000000000000}; //21.000.000e8  21e14
			cash_t block_reward{500000000}; //5e8
		};

		db_t db;

		virtual void dbhash(hasher_t&) const override;


		struct policies_t: blockchain::policies_t<double, policies_traits> {
			typedef blockchain::policies_t<double, policies_traits> b;
			policies_t() {
				temp_load();
			}
			void temp_load() {
				(*this)[minimum_fee]=1;
				//(*this)[lifestyle_minimum_level]=10000;
			}
		};
		void dump_policies(ostream& os) const;
		mutable mutex mx_policies;
		policies_t policies;
		policies_t policies_local;

	};


/*
//activities in bitcoin layer

//1.
  collect tx in tx_pool

//2. cycle of T=1 min
  wait for signal blockchain_ready

  //segment 1 - network exchange of nblocks
  new nblock_pool
  copy tx_pool, build my nblock
  send my nblock
  receive nblocks, add them to nblock_pool

  //segment 2 - local block settle
  nblock_pool::get_block() based on head
  add it to my local blockchain
*/
}
}



#endif

