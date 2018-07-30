#ifndef USGOV_49e5fff0395fac385e48353d91ae777f7b0ba92baa74a883f127855ad412c9e0
#define USGOV_49e5fff0395fac385e48353d91ae777f7b0ba92baa74a883f127855ad412c9e0

#include <us/gov/auth.h>
#include <us/gov/blockchain.h>

#include <vector>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <us/gov/socket/datagram.h>
#include <us/gov/signal_handler.h>
#include <us/gov/crypto/crypto.h>
#include <us/gov/blockchain/diff.h>
#include "protocol.h"
#include <chrono>
#include <cassert>
#include <map>
#include <atomic>

namespace us { namespace gov {
namespace cash {


	using namespace std;
	using socket::datagram;
	using blockchain::local_deltas;
	using crypto::ripemd160;
	using crypto::ec;
	using blockchain::peer_t;

	typedef crypto::ec::keys keys;
	typedef keys::pub_t pubkey_t;

	typedef ripemd160 hasher_t;
	typedef hasher_t::value_type hash_t;

	typedef int64_t cash_t; //signed!

	static const hash_t min_locking_program(10000);

    typedef uint32_t token_t;

    struct tx;

	struct app:blockchain::runnable_app {
		app();
		virtual ~app();


		struct policies_traits {
			enum paramid {
				minimum_fee=0,
//				floating_point_pos=1,
				num_params,
			};
			constexpr static array<const char*,num_params> paramstr={"minimum_fee"/*,"floating_point_pos"*/};
		};


		struct local_delta: blockchain::policies_local_delta<double, policies_traits> {
			typedef blockchain::policies_local_delta<double, policies_traits> b;

			local_delta& operator =(int zero);
			local_delta();
			virtual ~local_delta();
			virtual int app_id() const override;

			virtual void to_stream(ostream&) const override;
			virtual void from_stream(istream&) override;

            struct tokens_t:unordered_map<token_t,cash_t> {
	    			void dump(ostream& os) const;
		    		void to_stream(ostream& os) const;
			    	static tokens_t from_stream(istream& is);
            };

            struct safe_deposit_box {  //safe deposit box
                tokens_t balance;
                //SC data goes here

				void dump(ostream& os) const;
				void to_stream(ostream& os) const;
				static safe_deposit_box from_stream(istream& is);
            };

			struct account_t {
				account_t();
				account_t(const hash_t& locking_program, const cash_t& balance);

				hash_t locking_program;
				cash_t balance;  //deprecated, use sdb
                safe_deposit_box box;

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
			local_delta g;
			blockchain::majority_merger<local_delta>* m;
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
			datagram* get_datagram() const;
			static query_accounts_t from_datagram(datagram*);
			static query_accounts_t from_string(const string&);
		};

		virtual blockchain::app::local_delta* create_local_delta() override;

		virtual bool process_query(peer_t *, datagram*) override;

		virtual bool process_evidence(datagram*) override;
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
			bool add_(const hash_t&, const cash_t& amount);
			bool withdraw_(const hash_t& k, const cash_t& amount);

			void dump(ostream& os) const;
			cash_t get_newcash();
			void clear();
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
				//(*this)[floating_point_pos]=8;
			}
		};
		void dump_policies(ostream& os) const;
		mutable mutex mx_policies;
		policies_t policies;
		policies_t policies_local;

	};

}}
}

#endif
