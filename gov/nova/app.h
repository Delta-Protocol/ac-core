#ifndef USGOV_d73b1f5d97d3c08f466bf17e137750c217c3d4e354dab33d3a54a1f45657d8fa
#define USGOV_d73b1f5d97d3c08f466bf17e137750c217c3d4e354dab33d3a54a1f45657d8fa

#include <us/gov/auth.h>
#include <us/gov/blockchain.h>

#include <vector>
#include <thread>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
#include <us/gov/signal_handler.h>
#include <us/gov/crypto/crypto.h>
#include <us/gov/blockchain/diff.h>
#include "protocol.h"
#include <chrono>
#include <cassert>
#include <map>
#include <atomic>
#include "evidence_load.h"
#include "evidence_track.h"


namespace us{ namespace gov {
namespace nova {
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
				unused=0,
				num_params,
			};
			constexpr static array<const char*,num_params> paramstr={"unused"};
		};

		struct local_delta: blockchain::policies_local_delta<double, policies_traits> {
			typedef blockchain::policies_local_delta<double, policies_traits> b;

			local_delta& operator =(int zero);
			local_delta();
			virtual ~local_delta();
			virtual int app_id() const override;

			virtual void to_stream(ostream&) const override;
			virtual void from_stream(istream&) override;

			typedef chrono::system_clock::time_point time_point;
             
			struct logbook_t:vector<string> {
                typedef evidence_load::item_t item_t;

				unordered_set<item_t> items;
				void dump(ostream& os) const;
				void dump_brief(ostream& os) const;

                void rm(const item_t& item) {
                    auto i=items.find(item);
                    if (i==items.end()) return;
                    items.erase(i);
                }
                void add(const item_t& item) {
                    auto i=items.find(item);
                    if (i!=items.end()) return;
                    items.emplace(item);
                }
//				logbook_t& operator+=(const logbook_t& other) {
//
//				}
                void compute_hash(hasher_t&) const;
				void to_stream(ostream& os) const;
				static logbook_t from_stream(istream& is);
			};

			struct compartiment_t {  //account_t
				compartiment_t();
				compartiment_t(const hash_t& locking_program, const logbook_t& );

				hash_t locking_program;
				logbook_t logbook;

    			void compute_hash(hasher_t&) const;

				void dump(ostream& os) const;
				void dump_brief(ostream& os) const;
				void to_stream(ostream& os) const;
				static compartiment_t from_stream(istream& is);
			};

			typedef compartiment_t batch_item;
			struct batch_t: unordered_map<hash_t,batch_item> {
				void add(const hash_t& address, const batch_item& bi);
			};

			struct compartiments_t: map<hash_t,compartiment_t> {
				void add(const batch_t& batch);
				//cash_t get_balance() const;
				void to_stream(ostream& os) const;
				static compartiments_t* from_stream(istream& is);

				//bool add_input(tx& t, const hash_t& addr, const cash_t& amount);
				//bool add_output(tx& t, const hash_t& addr, const cash_t& amount, const hash_t& locking_program);
    			void compute_hash(hasher_t&) const;

				
				void dump(ostream& os) const;
				//bool pay(const hash_t& k, const cash_t& amount);
				//bool withdraw(const hash_t& k, const cash_t& amount);
			};

			compartiments_t compartiments;
//			cash_t fees{0};

			mutable hash_t hash{0};
			const hash_t& get_hash() const;
			hash_t compute_hash() const;
		};

		struct delta: blockchain::policies_delta<double, policies_traits, blockchain::average_merger<double>> {
			typedef blockchain::policies_delta<double, policies_traits, blockchain::average_merger<double>> b;
			typedef local_delta::compartiments_t compartiments_t;
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


		constexpr static const char* name={"nova"};
		virtual string get_name() const override { return name; }

		static int id() { return 80; }
		virtual int get_id() const override { return id(); }

		virtual string shell_command(const string& cmdline) override;

		double supply_function(double x0, double x, double xf) const;

		virtual void run() override;
		void add_policies();

		local_delta* pool{0};
		mutex mx_pool;

		struct query_compartiments_t:vector<hash_t> {
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
				return new socket::datagram(protocol::nova_compartiment_query,os.str());
			}
			static query_compartiments_t from_datagram(datagram*);
			static query_compartiments_t from_string(const string&);
		};

		virtual blockchain::app::local_delta* create_local_delta() override;

		virtual bool process_query(peer_t *, datagram*) override;

		virtual bool process_evidence(peer_t *, datagram*) override;
//		bool process_tx(peer_t *, datagram*);
		void compartiment_query(peer_t *, datagram*);

		virtual void import(const blockchain::app::delta&, const blockchain::pow_t&) override;


		typedef local_delta::compartiment_t compartiment_t;

		typedef local_delta::compartiments_t compartiments_t;



		bool fetch_compartiment(const local_delta::batch_t& batch, const hash_t& address, compartiment_t& acc) const;

		bool checksig(const string& locking_program_input, const evidence&) const;
		static bool unlock(const hash_t& address, const hash_t& locking_program, const string& locking_program_input, const evidence&);

		bool process(const evidence_load&);
		bool process(const evidence_track&);

		struct db_t {
			db_t();
			db_t(db_t&& other);
			~db_t();
			//bool add_(const hash_t&, const cash_t& amount);
			//bool withdraw_(const hash_t& k, const cash_t& amount/*, accounts_t::undo_t&*/);

			void dump(ostream& os) const;
			//cash_t get_newcash();
			void clear();
			void to_stream(ostream&) const;
			static db_t from_stream(istream&);

			mutable mutex mx;
			compartiments_t* compartiments{0};
			//cash_t supply_left;
			//cash_t block_reward;
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
				(*this)[unused]=0;
			}
		};
		void dump_policies(ostream& os) const;
		mutable mutex mx_policies;
		policies_t policies;
		policies_t policies_local;

	};


}
}}

#endif

