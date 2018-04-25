#ifndef USGOV_c13d19f0c3971de6c878039137bc456aee76c735c59394602b4d3ee39130825d
#define USGOV_c13d19f0c3971de6c878039137bc456aee76c735c59394602b4d3ee39130825d

#include <gov/net.h>
#include <gov/auth.h>
#include "diff.h"
#include "peer_t.h"
#include "app.h"
#include "auth_app.h"
#include "policies.h"
#include <gov/signal_handler.h>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <gov/dfs.h>

#include <vector>
#include <cassert>
#include <thread>
#include <chrono>
#include <ctime>
#include <condition_variable>
#include <mutex>
#include "shell.h"
//#include <queue>

namespace usgov {
namespace blockchain {
	using namespace std;
	using socket::datagram;

	struct cycle_t {
		typedef chrono::system_clock::duration duration;
		typedef chrono::system_clock::time_point time_point;
		typedef chrono::minutes minutes;
		typedef chrono::seconds seconds;
		enum stage {
			new_cycle=0,
			miner_gut_io=10,
			consensus_vote_tip_io=40,
			num_stages
		};
		
		cycle_t() {}
		void wait_for_stage(stage ts);
		string str(stage s) const;
		stage get_stage();

		time_point cur_sync;
		duration period{60s};
	};

	struct daemon {
		typedef app::keys keys;

		daemon(const keys&);
		daemon(const keys&, const string& blocksdir, uint16_t port, uint8_t num_edges,const vector<string>& seed_nodes);
		daemon(const daemon&)=delete;
		daemon(daemon&&)=delete;
		~daemon();

		void constructor();

		void add(app*app);

		static bool file_exists(const string& f);

		struct networking:dfs::daemon {
			typedef dfs::daemon b;
			using b::keys;
			networking(const keys& k, blockchain::daemon* parent): b(k), parent(parent) {}
			networking(const keys& k, uint16_t port, uint16_t edges, blockchain::daemon* parent, const vector<string>& seed_nodes): b(k, port, edges), parent(parent), seed_nodes(seed_nodes) {}
			virtual bool process_work(socket::peer_t *c, datagram*d) override;
			virtual bool process_evidence(peer::peer_t *c, datagram*d) override;
			bool process_work_sysop(peer::peer_t *c, datagram*d);
			virtual string get_random_peer(const unordered_set<string>& exclude) const override; //returns ipaddress //there exist a possibility of returning "" even though there were eligible items available

			virtual socket::client* create_client(int sock) override {
				auto p=new peer_t(sock);
				p->parent=this;
				return p;
			}
			void dump(ostream& os) const {
				os << "Networking. Active edges:" << endl;
				auto a=active();
				vector<peer_t*>& v=reinterpret_cast<vector<peer_t*>&>(a);
				for (auto& i:v) {
					i->dump(os);
				}
			}

			blockchain::daemon* parent;
			vector<string> seed_nodes;
		};

		bool process_work(peer_t *c, datagram*d);
		bool process_app_query(peer_t *c, datagram*d);
		bool process_evidence(peer_t *c, datagram*d);
		void process_query_block(peer_t *c, datagram*d);
		void process_block(peer_t *c, datagram*d);
		void process_vote_tip(peer_t *c, datagram*d);

		void relay(int num, peer_t* exclude, datagram* d) {
			peerd.send(num,exclude,d);
		}

		int miners_size() const {
			return 30; //TODO based on the number of miner_guts a block has
		}

		void vote_tip(const diff& b);
		void dump(ostream& os) const;
		void list_apps(ostream& os) const;
		string load_block(const diff::hash_t& hash) const;
		string load_block(const string& block_hash_b58) const;

		struct syncd: usgov::signal_handler::callback {
			typedef diff::hash_t hash_t;
			syncd(daemon* d);
	
			void dump(ostream& os) const;

			void run();
			void update(const hash_t& head, const hash_t& tail);
			void update(const hash_t& tail);
			void update();
			void wait();
			void wait(const chrono::steady_clock::duration& d);
			virtual void on_finish();

			daemon* d;
			condition_variable cv;
			mutable mutex mx;
			bool in_sync() const;
			hash_t head;
			hash_t cur;
			hash_t tail;
			bool resume{false};
		};
/*
		struct chain: array<pair<diff::hash_t,diff::hash_t>,10> { // { {base,diff} }
			
		};
*/
		bool need_sync(const string& target) const;
		void sync(const string& target);

		void process_incoming_miner_gut(peer_t *c, datagram*d);

		bool sysop_allowed{false};
		bool get_prev(const diff::hash_t& h, diff::hash_t& prev) const;

		void send(const miner_gut& g, peer_t* exclude=0);
		void send(const datagram& g, peer_t* exclude=0);

		struct cycle_data {
			cycle_data():new_block(0) {
			}
			~cycle_data() {
				delete new_block;
			}
			diff* new_block;
			cycle_t cycle;
		};

		void stage1(cycle_data&);
		bool stage2(cycle_data&);
		void stage3(cycle_data&);
		void run();

		void load_head();

		void on_begin_cycle();
		miner_gut* create_miner_gut();

		void update_peers_state();



		bool import(const diff& b);
		diff::hash_t last_block_imported;
		mutable mutex mx_import;
		diff::hash_t get_last_block_imported() const;
		void set_last_block_imported(const diff::hash_t&);
		void set_last_block_imported_(const diff::hash_t&);
		unsigned int get_seed() const;

		string shell_command(int app_id, const string& cmdline) const;

		void query_block(const diff::hash_t& hash);
		void save(const diff& bl) const;

		struct apps:unordered_map<int,app*> {
			virtual ~apps() { for (auto i:*this) delete i.second; }
			void dump(ostream& os) const;
			string shell_command(int app_id, const string& cmdline) const;

		};

		vector<peer_t*> get_nodes();
		vector<peer_t*> get_people();

		bool patch_db(const vector<diff::hash_t>& patches);

		const diff::hash_t& dbhash_off() const;

		mutable diff::hash_t cached_dbhash;
		mutable bool cached_dbhash_ok{false};

		peer_t* get_random_node();

		networking peerd;

		diff* pool{0};
		mutable mutex mx_pool;

		apps apps_;
		blockchain::auth::app* auth_app;

		struct votes_t:unordered_map<pubkey_t::hash_t,pair<diff::hash_t,unsigned long>> { // <pubkey,pair<hash,count>>
			typedef unordered_map<pubkey_t::hash_t,pair<diff::hash_t,unsigned long>> b;
			void clear();
			bool add(const pubkey_t::hash_t&,const diff::hash_t& v);
			diff::hash_t select();
			mutex mx;
		};
		
		syncd syncdemon;

		struct sysops_t:unordered_map<peer_t *,shell> {
			sysops_t(const daemon&d): d(d) {}
			bool process_work(peer_t *p, datagram*d);
			mutex mx;
			const daemon& d;
		};

		sysops_t sysops;


		votes_t votes;
		string blocksdir;

		mutable mt19937_64 rng;

	};

}
}

#endif

