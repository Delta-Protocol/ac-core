#ifndef USGOV_49e5fff0395fac385e48353d91ae777f7b0ba92baa74a883f127855ad412c9e0
#define USGOV_49e5fff0395fac385e48353d91ae777f7b0ba92baa74a883f127855ad412c9e0

#include <gov/auth.h>
#include <gov/blockchain.h>

#include <vector>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <gov/socket/datagram.h>
#include <gov/signal_handler.h>
#include <gov/crypto/crypto.h>
#include <gov/blockchain/diff.h>
#include "protocol.h"
#include <chrono>
#include <cassert>
#include <map>
#include <atomic>
#include "tx.h"


namespace us { namespace gov {
namespace cash {
	using namespace std;
	using socket::datagram;
	//using blockchain::signature;
	using blockchain::local_deltas;
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



	struct app:blockchain::runnable_app { 
		app();
		virtual ~app();


		struct policies_traits {
			enum paramid { 
				minimum_fee=0,
				num_params,
			};
			constexpr static array<const char*,num_params> paramstr={"minimum_fee"/*,"lifestyle_minimum_level"*/};
		};

		struct local_delta: blockchain::policies_local_delta<double, policies_traits> {
			typedef blockchain::policies_local_delta<double, policies_traits> b;

			local_delta& operator =(int zero);
			local_delta();
			virtual ~local_delta();
			virtual int app_id() const override;

			virtual void to_stream(ostream&) const override;
			virtual void from_stream(istream&) override;

			struct account_t {
				account_t();
				account_t(const hash_t& locking_program, const cash_t& balance);

				hash_t locking_program;
				cash_t balance;

				void dump(ostream& os) const;
				void to_stream(ostream& os) const;
				static account_t from_stream(istream& is);
			};

			typedef account_t batch_item;
			struct batch_t: unordered_map<hash_t,batch_item> {
				void add(const hash_t& address, const batch_item& bi);
			};

			struct accounts_t: map<hash_t,account_t> {
				void add(const batch_t& batch);
				cash_t get_balance() const;
				void to_stream(ostream& os) const;
				static accounts_t* from_stream(istream& is);

				bool add_input(tx& t, const hash_t& addr, const cash_t& amount);
				bool add_output(tx& t, const hash_t& addr, const cash_t& amount, const hash_t& locking_program);
				
				void dump(ostream& os) const;
				bool pay(const hash_t& k, const cash_t& amount);
				bool withdraw(const hash_t& k, const cash_t& amount);
			};

			accounts_t accounts; 
			cash_t fees{0};

			mutable hash_t hash{0};
			const hash_t& get_hash() const;
			hash_t compute_hash() const;
		};

		struct delta: blockchain::policies_delta<double, policies_traits, blockchain::average_merger<double>> {
			typedef blockchain::policies_delta<double, policies_traits, blockchain::average_merger<double>> b;
			typedef local_delta::accounts_t accounts_t;
			delta();
			virtual ~delta();
			virtual uint64_t merge(blockchain::app::local_delta* other0) override;
			virtual void end_merge() override;
			virtual void to_stream(ostream& os) const override;
			static delta* from_stream(istream& is);
	//		map<pubkey,address> to_hall; //pubkey
	//		array<double,policies_traits::num_params> policies;
			local_delta g;
			blockchain::majority_merger<local_delta>* m;
			//cash_t fees{0};
		};


		constexpr static const char* name={"cash"};
		virtual string get_name() const override { return name; }

		static int id() { return 30; }
		virtual int get_id() const override { return id(); }

		virtual string shell_command(const string& cmdline) override;

		double supply_function(double x0, double x, double xf) const;

		virtual void run() override;
		void add_policies();

		local_delta* pool{0};
		mutex mx_pool;

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

		virtual blockchain::app::local_delta* create_local_delta() override;

		virtual bool process_query(peer_t *, datagram*) override;

		virtual bool process_evidence(peer_t *, datagram*) override;
		bool process_tx(peer_t *, datagram*);
		void cash_query(peer_t *, datagram*);

		virtual void import(const blockchain::app::delta&, const blockchain::pow_t&) override;


		typedef local_delta::account_t account_t;

		typedef local_delta::accounts_t accounts_t;



		bool account_state(const local_delta::batch_t& batch, const hash_t& address, account_t& acc) const;

		bool checksig(const size_t& this_index, const string& locking_program_input, const tx&) const;
		static bool unlock(const hash_t& address, const size_t& this_index, const hash_t& locking_program, const string& locking_program_input, const tx&);

		bool process(const tx&);

		struct db_t {
			db_t();
			db_t(db_t&& other);
			~db_t();
			//bool move(const string& src, const string& dst, uint64_t amount);
			//bool move(const tx::end_t& srcs, const tx::end_t& dsts);
			bool add_(const hash_t&, const cash_t& amount);
//			bool withdraw_(unsigned int seed, const spend_code_t& sc, const hash_t& k, const cash_t& amount, accounts_t::undo_t&);
			bool withdraw_(const hash_t& k, const cash_t& amount/*, accounts_t::undo_t&*/);

			void dump(ostream& os) const;
			//void clear();
			cash_t get_newcash();
			void clear();
			//typedef unordered_map<string,cash_t> accounts_t;
			void to_stream(ostream&) const;
			static db_t from_stream(istream&);

			mutable mutex mx;
			accounts_t* accounts{0};
			cash_t supply_left;
			cash_t block_reward;
		};

		db_t db;

		virtual void dbhash(hasher_t&) const override;
		virtual void clear() override;


		struct policies_t: blockchain::policies_t<double, policies_traits> {
			typedef blockchain::policies_t<double, policies_traits> b;
			policies_t() {
				temp_load();
			}
			void temp_load() {
				(*this)[minimum_fee]=1;
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
}}
}



#endif

