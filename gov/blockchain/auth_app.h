#ifndef USGOV_1889feb46c109de0a04e33395e575f90dccd36322bcf52a8e6c93042928b612a
#define USGOV_1889feb46c109de0a04e33395e575f90dccd36322bcf52a8e6c93042928b612a

#include "app.h"
#include "peer_t.h"
#include "policies.h"
//#include "diff.h"
#include <string>
#include <gov/crypto/hash.h>
#include <map>
#include <set>
#include <mutex>
#include <random>
#include <unordered_set>

namespace usgov {
namespace blockchain {

struct peer_t;

namespace auth { //blockchain network support, this service is free
		//IP4 ownership, held for minimum 1 day, maximum 30 days, renew only possible last day
	using namespace std;
	using socket::datagram;
	typedef chrono::system_clock::time_point time_point;



//	enum transition_t {
//		to_hall, to_node, to_out //, remain
//	};
/*
	struct tx {
		tx() {
		}
		~tx() {
		}
		void to_stream(ostream&) const;
		static tx from_stream(istream&);
		/ *
		crypto::sha256::value_type compute_hash() const {
			ostringstream os;
			to_stream(os);
			return crypto::sha256::compute(os.str());
		}
* /
		transition_t transition;
		string pubkey;
		string address;
	};
*/
/*
	struct tx_policies {
		tx_policies() {
		}
		~tx_policies() {
		}
		void to_stream(ostream&) const;
		static tx_policies from_stream(istream&);
		vector<pair<int,double>> diffs;
	};
*/
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
/*
		virtual bool process_work(peer_t *c, datagram*d) override {
			return false;
		}
*/
		peer_t::stage_t my_stage() const;
		mutable peer_t::stage_t cache_my_stage{peer_t::unknown};

		bool is_node() const { return my_stage()==peer_t::node; }
//		virtual bool in_service() const override; //is this node in service (i.e. does it belong to the network?)

//		virtual void on_begin_cycle() override;

//		virtual void on_head_ready() override { //can start verification
//		}

		virtual void import(const blockchain::app::delta&, const blockchain::pow_t&) override;
		//void import(const tx&);

		virtual blockchain::app::local_delta* create_local_delta() override;

//		virtual blockchain::app_gut* create_closure_gut(const blockchain::block&) override;

		string get_ip4() const {
		}

/*
		void register_renew() {		//miner running this instance wants to renew, or register if first time
			tx* t=new tx();
			string ip4;
			t->owner=pk;
			t->ip4=get_ip4(); //I claim to be reachable at this address, I'll be contacted
			pool->push_back(t); //needs sync
		}
*/
			//void revoke() { //free the ip4 resource, can only be done in the last day before granted period expires#
			//}

		local_delta* pool{0};
		mutex mx_pool;

	//	time_point issued;
	//	time_point expiry;

		
		string get_random_node(mt19937_64& rng, const unordered_set<string>& exclude_addrs) const; //there exist a possibility of returning "" even though there were eligible items available

//		virtual void clear_db() override;

		struct db_t {
			db_t() {
//				temp_fill();
			}
/*
			void temp_fill() {
				lock_guard<mutex> lock(mx_nodes);
				nodes.emplace(pubkey_t::from_b58("wC8dAa6V9gJC9bMgqobkDM9Wqy6bxtxYk95BYJzKyMyE").hash(),"--.--.--.--");
			}
*/
/*
            void genesis(const pubkey_t& k, const address& addr) {
				lock_guard<mutex> lock(mx_nodes);
                assert(nodes.empty());
				nodes.emplace(k.hash(),addr);
            }
*/

			peer_t::stage_t get_stage(const pubkeyh_t&) const;

			void hash(hasher_t&) const;
			//void clear();

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
/*
		struct average {
		};
		template<typename T, typename method>
		struct consensuated_variable {
		};
*/
//		consensuated_variable<double,average> v;
		//struct consensuated_variable {
		//};
/*
		template<typename T, typename DT=T>
		struct consensuated_variable {
			virtual ~consensuated_variable() {}
			T value;
			T vote;
			DT local_diff() {
				DT d=vote-value;
				return move(d);
			}
			virtual string consensus_type() const=0; 
			
		};
		template<typename T, typename DT=T>
		struct consensuated_variable_average: consensuated_variable<T,DT> {
			virtual ~consensuated_variable_average() {}
			virtual string consensus_type() const { return "average"; }; 
			
		};
		template<typename E, typename T, typename DT=T>
		struct consensuated_array_average: vector<consensuated_variable_average<T,DT>> {
			typedef vector<consensuated_variable_average<T,DT>> b;
			consensuated_array_average() {
				b::resize(E::num_params);
			}

			mutable mutex mx;
			T get_value(const E& p) const {
				lock_guard<mutex> lock(mx);
				return (*this)[p].value;
			}
			T get_vote(const E& p) const {
				lock_guard<mutex> lock(mx);
				return (*this)[p].vote;
			}

			void set(const E& p, const T& v, const T& vote) {
				lock_guard<mutex> lock(mx);
				(*this)[p].value=v;
				(*this)[p].vote=vote;
			}
			void set_vote(const E& p, const T& v) {
				lock_guard<mutex> lock(mx);
				(*this)[p].vote=v;
			}
			void set_value(const E& p, const T& v) {
				lock_guard<mutex> lock(mx);
				(*this)[p].value=v;
			}

			vector<DT> local_diff() const {
				vector<DT> v;
				lock_guard<mutex> lock(mx);
				for (size_t i=0; i<E::num_params; ++i) {
					v.emplace_back(*this[i].local_diff());
				}
				return move(v);
			}
		};
*/
		
		pubkey_t node_pubkey;


		db_t db;

		virtual void clear() override;


		const keys& get_keys() const;


//		struct policies_t: consensuated_array_average<paramid,double> { // network value, my vote
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

//			unordered_map<string,string> majority; //consensus is the most common value
//			 average; //consensus is the average value

			//void clear();
/*
			void dump(ostream& os) const;
			mutable mutex mx;
			struct data {
				void set(param_t::consensus_t ct,const double& val, const double& vo) {
					consensus_type=ct;
					value=val;
					vote=vo;
				}
				double value, vote;
				param_t::consensus_t consensus_type;
			};
			data db[num_params];
		};
*/

	};


}
}
}
#endif

