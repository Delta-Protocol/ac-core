#ifndef USGOV_c13d19f0c3971de6c878039137bc456aee76c735c59394602b4d3ee39130825d
#define USGOV_c13d19f0c3971de6c878039137bc456aee76c735c59394602b4d3ee39130825d

#include <us/gov/auth.h>
#include "diff.h"
#include "peer_t.h"
#include "app.h"
#include "auth_app.h"
#include "policies.h"
#include "cycle.h"
#include <us/gov/signal_handler.h>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <us/gov/dfs.h>

#include <vector>
#include <cassert>
#include <thread>
#include <chrono>
#include <ctime>
#include <condition_variable>
#include <mutex>
#include "shell.h"
#include "networking.h"
#include "syncd.h"


namespace us { namespace gov {
namespace engine {
	using namespace std;
	using socket::datagram;
    using namespace std::chrono;


	struct daemon {
		typedef crypto::ec::keys keys;

		daemon(const keys&);
		daemon(const keys&, const string& home, uint16_t port, uint8_t num_edges,const vector<string>& seed_nodes);
		daemon(const daemon&)=delete;
		daemon(daemon&&)=delete;
		~daemon();

		void constructor();

		void add(app*app);

		static bool file_exists(const string& f);

        string blocksdir() const;

        void eat_diff(const diff::hash_t& voted_tip, cycle_t& cycle);

		bool process_work(peer_t *c, datagram*d);
		bool process_app_query(peer_t *c, datagram*d);
		bool process_evidence(datagram*d);
		void process_query_block(peer_t *c, datagram*d);
		void process_block(peer_t *c, datagram*d);
		void process_vote_tip(peer_t *c, datagram*d);

		void relay(int num, peer_t* exclude, datagram* d) {
			peerd.send(num,exclude,d);
		}

        template<typename T>
        struct hold: unordered_set<T*> {
            hold() {  
            }
            ~hold() {  
                for (auto p:*this) delete p;
            }
            void add(T* d) {
                unique_lock<mutex> lock(mx);
                this->emplace(d);
            }
            void wait_empty() const {
                unique_lock<mutex> lock(mx);
                if(this->empty()) return;
                cv.wait(lock,[&]{ return this->empty() || program::_this.terminated; });
            }
            mutable mutex mx;
            mutable condition_variable cv;
        };
        hold<datagram> evidences_on_hold;

        struct calendar_t: multimap<uint64_t, pair<evidence*, app*>> { //TODO implement < operator to solve deterministicly the order, in the unli$
            
            ~calendar_t() {
                for (auto&i:*this) delete i.second.first;
            }
            
            void schedule(evidence * e, app* p) {
                lock_guard<mutex> lock(mx);
                emplace(e->ts,make_pair(e,p));
            }

            mutex mx;
        };

        calendar_t calendar;

        void on_sync();
        void flush_evidences_on_hold();

        void dbg_output() const;
		void vote_tip(const diff& b);
		void dump(ostream& os) const;
		void list_apps(ostream& os) const;
		string load_block(const diff::hash_t& hash) const;
		string load_block(const string& block_hash_b58) const;
        void evidence_processor();

		bool need_sync(const string& target) const;
		void sync(const string& target);

		void process_incoming_local_deltas(peer_t *c, datagram*d);

		bool sysop_allowed{false};
		bool get_prev(const diff::hash_t& h, diff::hash_t& prev) const;

		void send(const local_deltas& g, peer_t* exclude=0);

        cycle_t cycle;

		void stage1(cycle_t&);
		void stage2(cycle_t&);
		bool stage3(cycle_t&);
		void stage4(cycle_t&);
		void run();

		void load_head();

		local_deltas* create_local_deltas();

		void update_peers_state();

        void clear(); //clear database

		bool process_sysop_request(peer_t *c, datagram*d);


		bool import(const diff& b);
		diff::hash_t last_block_imported;
		mutable mutex mx_import;
		diff::hash_t get_last_block_imported() const;
		void set_last_block_imported(const diff::hash_t&);
		void set_last_block_imported_(const diff::hash_t&);

		string shell_command(int app_id, const string& cmdline) const;

		peer_t* query_block(const diff::hash_t& hash); //returns the peer where the block was queryed to
		void save(const diff& bl) const;

		struct apps:unordered_map<int,app*> {
			virtual ~apps() { for (auto i:*this) delete i.second; }
			void dump(ostream& os) const;
			string shell_command(int app_id, const string& cmdline) const;

		};

		vector<peer_t*> get_nodes();
		vector<peer_t*> get_people();

		bool patch_db(const vector<diff::hash_t>& patches);

		string get_random_node(const unordered_set<string>& exclude_addrs) const;

		const diff::hash_t& dbhash_off() const;

		mutable diff::hash_t cached_dbhash;
		mutable bool cached_dbhash_ok{false};

		peer_t* get_random_edge();

		networking peerd;

		diff* pool{0};
		mutable mutex mx_pool;

		apps apps_;
		auth::app* auth_app;

		void print_performances(ostream&) const;
		string timestamp() const;

//        void start_new_blockchain(const string& addr);

		struct votes_t:unordered_map<pubkey_t::hash_t,pair<diff::hash_t,unsigned long>> { // <pubkey,pair<hash,count>>
			typedef unordered_map<pubkey_t::hash_t,pair<diff::hash_t,unsigned long>> b;
			void clear();
			bool add(const pubkey_t::hash_t&,const diff::hash_t& v);
			diff::hash_t select();
			mutex mx;
		};
		
		syncd_t syncd;

		struct sysops_t:unordered_map<peer_t *,shell> {
			sysops_t(daemon&d): d(d) {}
			bool process_work(peer_t *p, datagram*d);
			mutex mx;
			daemon& d;
		};

		sysops_t sysops;


		votes_t votes;
		string home;

		mutable mt19937_64 rng;

        keys id;

	};

}}
}

#endif

