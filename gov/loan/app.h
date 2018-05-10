#ifndef USGOV_51d4393eec61d2ef445fd79c7b25d5d8a548fe50d2c4e798f97944ce8faf3208
#define USGOV_51d4393eec61d2ef445fd79c7b25d5d8a548fe50d2c4e798f97944ce8faf3208

#include <gov/auth.h>
#include <gov/blockchain.h>

#include <vector>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <gov/signal_handler.h>
#include <gov/crypto/crypto.h>
#include <chrono>
#include <cassert>
#include <map>
#include <gov/crypto/hash.h>

namespace usgov {
namespace loan {
	using namespace std;
	using socket::datagram;
//	using blockchain::signature;
	using blockchain::local_deltas;
	using crypto::ripemd160;
	using blockchain::peer_t;

	typedef int64_t cash_t;

		typedef crypto::ripemd160 hasher_t;
		typedef hasher_t::value_type hash_t;

		struct policies_traits {
			enum paramid {
				next_patch=0,
				num_params,
			};
			constexpr static array<const char*,num_params> paramstr={"next_patch"};
		};

	struct local_delta: blockchain::policies_local_delta<hash_t, policies_traits> {
		typedef blockchain::policies_local_delta<hash_t, policies_traits> b;

		local_delta() {
		}
		virtual ~local_delta() {
			//for (auto i:*this) delete i; // delete checkpoint_; 
		}
		virtual int app_id() const override;

		virtual void to_stream(ostream&) const override;
		virtual void from_stream(istream&) override;

//		typedef unordered_map<string,cash_t> accounts_t;
//		accounts_t accounts;
		cash_t fees{0};

	};

	struct delta: blockchain::policies_delta<hash_t, policies_traits, blockchain::majority_merger<hash_t>> {
		typedef blockchain::policies_delta<hash_t, policies_traits, blockchain::majority_merger<hash_t>> b;
		delta() {}
		virtual ~delta() {
		}
		virtual uint64_t merge(blockchain::app::local_delta* other0) override {

			local_delta* other=static_cast<local_delta*>(other0);
			auto val=other->fees;
			b::merge(other0);
			return val;

		}
		virtual void to_stream(ostream& os) const override;
		static delta* from_stream(istream& is);
		cash_t fees{0};
	};

	struct app:blockchain::app {
		app();
		virtual ~app();
		constexpr static const char* name={"rep"};
		virtual string get_name() const override { return name; }

//		virtual void on_begin_cycle() override;

		static int id() { return 40; }
		virtual int get_id() const override { return id(); }

		virtual string shell_command(const string& cmdline) override;

		double supply_function(double x0, double x, double xf) const;

//		virtual void run() override;
		void add_policies();

		loan::local_delta* pool{0};
		mutex mx_pool;

		virtual blockchain::app::local_delta* create_local_delta() override;
		//virtual bool process_work(peer_t *c, datagram*d) override;
		virtual void import(const blockchain::app::delta&, const blockchain::pow_t&) override;
//		bool process(const tx&);

		struct db_t {
			db_t() {
			}
			typedef ripemd160 hasher_t;
			typedef hasher_t::value_type hash_t;
//			void add_(const hash_t&, cash_t& amount);

			void dump(ostream& os) const;
			cash_t get_newcash();

			typedef unordered_map<hash_t,cash_t> accounts_t;

			mutable mutex mx;
//			accounts_t accounts;
//			cash_t supply_left{ 2100000000000}; //trusted variable
//			cash_t block_reward{    500000000}; //trusted variable
		};

		db_t db;

		virtual void dbhash(hasher_t&) const override;

		struct policies_t: blockchain::policies_t<hash_t, policies_traits> {
			typedef blockchain::policies_t<hash_t, policies_traits> b;
			policies_t() {
				temp_load();
			}
			void temp_load() {
				(*this)[next_patch]=0;
			}
		};
		void dump_policies(ostream& os) const;
		mutable mutex mx_policies;
		policies_t policies;
		policies_t policies_local;

	};


}
}


#endif

