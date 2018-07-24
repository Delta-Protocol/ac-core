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


namespace us { namespace gov {
namespace blockchain {
	using namespace std;
	using socket::datagram;

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

		struct networking:dfs::daemon {
			typedef dfs::daemon b;
			using b::keys;
			networking(blockchain::daemon* parent): parent(parent) {}
			networking(uint16_t port, uint16_t edges, blockchain::daemon* parent, const vector<string>& seed_nodes): b(port, edges), parent(parent), seed_nodes(seed_nodes) {}
			virtual bool process_work(socket::peer_t *c, datagram*d) override;
			virtual bool process_evidence(datagram*d) override;
			bool process_work_sysop(peer::peer_t *c, datagram*d);
			virtual string get_random_peer(const unordered_set<string>& exclude) const override; //returns ipaddress //there exist a possibility of returning "" even though there were eligible items available


            virtual const keys& get_keys() const override {
                return parent->id;
            }

            virtual vector<relay::peer_t*> get_nodes() override {
                auto a=parent->get_nodes();
                return *reinterpret_cast<vector<relay::peer_t*>*>(&a);
            }

			virtual socket::client* create_client(int sock) override {
				auto p=new peer_t(sock);
				p->parent=this;
				return p;
			}
			void dump(ostream& os) const {
				os << "Active edges:" << endl;
				auto a=active();
				vector<peer_t*>& v=reinterpret_cast<vector<peer_t*>&>(a);
				for (auto& i:v) {
					i->dump_all(os);
				}
			}

			blockchain::daemon* parent;
			vector<string> seed_nodes;
		};

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
        struct mxvector: vector<T*> {
            mxvector() {  
            }
            ~mxvector() {  
                for (auto p:*this) delete p;
            }
            void add(T* d) {
                unique_lock<mutex> lock(mx);
                this->emplace_back(d);
            }
            mutex mx;
        };
        mxvector<datagram> evidences_on_hold;


        void on_sync();
		void vote_tip(const diff& b);
		void dump(ostream& os) const;
		void list_apps(ostream& os) const;
		string load_block(const diff::hash_t& hash) const;
		string load_block(const string& block_hash_b58) const;

		struct syncd: us::gov::signal_handler::callback {
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

			void signal_file_arrived();

		        condition_variable cv_wait4file;
		        mutex mx_wait4file;
			bool file_arrived{false};


		};

		bool need_sync(const string& target) const;
		void sync(const string& target);

		void process_incoming_local_deltas(peer_t *c, datagram*d);

		bool sysop_allowed{false};
		bool get_prev(const diff::hash_t& h, diff::hash_t& prev) const;

		void send(const local_deltas& g, peer_t* exclude=0);
//		void send(const datagram& g, peer_t* exclude=0);

		void stage1(cycle_t&);
		bool stage2(cycle_t&);
		void stage3(cycle_t&);
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
		
		syncd syncdemon;

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

