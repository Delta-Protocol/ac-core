#ifndef USGOV_1889feb46c109de0a04e33395e575f90dccd36322bcf52a8e6c93042928b612a
#define USGOV_1889feb46c109de0a04e33395e575f90dccd36322bcf52a8e6c93042928b612a

#include "app.h"
#include "peer_t.h"
#include "policies.h"
//#include "diff.h"
#include <string>
#include <us/gov/crypto/hash.h>
#include <map>
#include <set>
#include <mutex>
#include <random>
#include <unordered_set>

namespace us { namespace gov {
namespace blockchain {

struct peer_t;

namespace auth { //blockchain network support, this service is free
		//IP4 ownership, held for minimum 1 day, maximum 30 days, renew only possible last day
	using namespace std;
	using socket::datagram;
	typedef chrono::system_clock::time_point time_point;

	typedef string address;
	typedef crypto::ec::keys keys;
	typedef keys::pub_t pubkey_t;
	typedef pubkey_t::hash_t pubkeyh_t;

	struct policies_traits {
		enum paramid {
			network_growth=0,
			network_min_growth,
			num_params,
		};
		constexpr static array<const char*,num_params> paramstr={"network_growth","network_min_growth"};
	};


	struct app: blockchain::runnable_app {
		typedef blockchain::app b;
		app(pubkey_t&);
		virtual ~app();

		struct local_delta: blockchain::policies_local_delta<double, policies_traits> {
			typedef blockchain::policies_local_delta<double, policies_traits> b;
			virtual ~local_delta() {
			}
			virtual int app_id() const override;
			virtual void to_stream(ostream& os) const override;
			virtual void from_stream_prev(istream& is) override;
			virtual void from_stream(istream& is) override;
			vector<pair<pubkeyh_t,address>> to_hall; //pubkey
		};

		struct delta: blockchain::policies_delta<double, policies_traits, blockchain::average_merger<double>> {
			typedef blockchain::policies_delta<double, policies_traits, blockchain::average_merger<double>> b;
			delta() {}
			virtual ~delta() {
			}
			virtual uint64_t merge(blockchain::app::local_delta* other0) override {
				local_delta* other=static_cast<local_delta*>(other0);
				for (auto& i:other->to_hall) to_hall.emplace(i);
				b::merge(other0);
				return 0;
			}
			virtual void to_stream(ostream& os) const override;
			static delta* from_stream_prev(istream& is);
			static delta* from_stream(istream& is);
			map<pubkeyh_t,address> to_hall; //pubkey
		};

		constexpr static const char* name={"auth"};
		virtual string get_name() const override { return name; }

		static int id() { return 20; }
		virtual int get_id() const override { return id(); }

		virtual void run() override;
		virtual string shell_command(const string& cmdline) override;

		virtual void dbhash(hasher_t&) const override;

		void basic_auth_completed(peer_t* p);

		void add_growth_transactions(unsigned int seed);
		void add_policies();

		peer_t::stage_t my_stage() const;
		mutable peer_t::stage_t cache_my_stage{peer_t::unknown};

		bool is_node() const { return my_stage()==peer_t::node; }

		virtual void import(const blockchain::app::delta&, const blockchain::pow_t&) override;

		virtual blockchain::app::local_delta* create_local_delta() override;

//		string get_ip4() const {
//		}

		local_delta* pool{0};
		mutex mx_pool;

		string get_random_node(mt19937_64& rng, const unordered_set<string>& exclude_addrs) const; //there exist a possibility of returning "" even though there were eligible items available

		struct db_t {
			db_t() {
			}

			peer_t::stage_t get_stage(const pubkeyh_t&) const;

			void hash(hasher_t&) const;

			void dump(ostream& os) const;
			typedef unordered_map<pubkeyh_t,address> nodes_t; //TODO, debe ser set<pubkey>
			typedef unordered_map<pubkeyh_t,address> hall_t; //TODO: guardar integer score, incrementar cada ciclo, net growth: solo seleccionar con score> 1200 
//			typedef map<pubkey,ip_address> hall_t; //TODO: guardar integer score, incrementar cada ciclo, net growth: solo seleccionar con score> 1200 
//Para una buena mezcla evil/honest se trata de consumir una IP mientras se esta en hall, hay que estar incrementando el score durante 1 dia para ser seleccionado.

            void clear();

			mutable mutex mx_nodes;
			nodes_t nodes;

			mutable mutex mx_hall;
			hall_t hall;
		};
		
		const pubkey_t& node_pubkey;

		db_t db;

		virtual void clear() override;


		const keys& get_keys() const;


		struct policies_t: blockchain::policies_t<double, policies_traits> {
			typedef blockchain::policies_t<double, policies_traits> b;
			policies_t() {
				temp_load();
			}
			void temp_load() {
				(*this)[network_growth]=0.01; // +ve, % (over num nodes) of people in hall will be accepted as nodes; -ve % of nodes will be degraded to hall
				(*this)[network_min_growth]=1.0;// >=0 ; only affects when network_growth>0
			}

		};
		void dump_policies(ostream& os) const;
		mutable mutex mx_policies;
		policies_t policies;
		policies_t policies_local;
	};


}
}}

}
#endif

