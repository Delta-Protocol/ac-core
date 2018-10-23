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


namespace us{ namespace gov{ namespace engine{
using namespace std;
using socket::datagram;
using namespace std::chrono;

class daemon {
public:
    typedef crypto::ec::keys keys;

    daemon(const keys&, const string& home, 
           uint16_t port, uint8_t num_edges,const 
           vector<string>& seed_nodes);
    daemon(const daemon&)=delete;
    daemon(daemon&&)=delete;
    ~daemon();

    void constructor();

    void add(app*app);

    static bool file_exists(const string& f);

    void eat_diff(const diff::hash_t& voted_tip, cycle_t& cycle);

    inline const dfs::daemon& dfs() const { 
        return m_peerd; 
    }
    inline dfs::daemon& dfs() { 
        return m_peerd; 
    }

public:
    bool process_work(peer_t *c, datagram*d);
    bool process_app_query(peer_t *c, datagram*d);
    bool process_evidence(datagram*d);

    void on_sync();
    void list_apps(ostream& os) const;
    bool sysop_allowed{false};
    bool get_prev(const string& filename, diff::hash_t& prev) const;
    string shell_command(int app_id, const string& cmdline) const;
    void clear();

    template<typename T>
    struct hold: unordered_set<T*> {
        hold() {}
        ~hold() {  
            for (auto p:*this) 
                delete p;
        }

        void add(T* d) {
            unique_lock<mutex> lock(mx);
            this->emplace(d);
        }

        void wait_empty() const {
            unique_lock<mutex> lock(mx);
            if(this->empty()) 
                return;
            cv.wait(lock,[&]{ return this->empty() || program::_this.terminated; });
        }
        mutable mutex mx;
        mutable condition_variable cv;
    };
 
    struct calendar_t: multimap<uint64_t, pair<evidence*, app*>> { 
            
        ~calendar_t() {
            for (auto&i:*this) 
                delete i.second.first;
        }
            
        void schedule(evidence * e, app* p) {
            lock_guard<mutex> lock(mx);
            emplace(e->get_ts(),make_pair(e,p));
        }

        mutex mx;
    };


    struct apps:unordered_map<int,app*> {
        virtual ~apps() { 
            for (auto i:*this) 
                delete i.second; 
        }
        void dump(ostream& os) const;
        string shell_command(int app_id, const string& cmdline) const;
    };

    vector<peer_t*> get_nodes() const;
    bool patch_db(const vector<diff::hash_t>& patches);
    string get_random_node(const unordered_set<string>& exclude_addrs) const;
    peer_t* get_random_edge(const peer_t* exclude) const;
    peer_t* get_random_edge() const;
    void print_performances(ostream&) const;

    void run();
private:
    void process_vote_tip(peer_t *c, datagram*d);
    void relay(int num, peer_t* exclude, datagram* d) {
        m_peerd.send(num,exclude,d);
    }

private:
    void flush_evidences_on_hold();
    void dbg_output() const;
    void vote_tip(const diff& b);
    void dump(ostream& os) const;

private:
    void evidence_processor();
    bool need_sync(const string& target) const;
    void sync(const string& target);
    void process_incoming_local_deltas(peer_t *c, datagram*d);


private:
    void send(const local_deltas& g, peer_t* exclude=0);
    void stage1(cycle_t&);
    void stage2(cycle_t&);
    bool stage3(cycle_t&);
    void stage4(cycle_t&);

private:
    void load_head();
    local_deltas* create_local_deltas();
    void update_peers_state();

private:
    bool process_sysop_request(peer_t *c, datagram*d);
    bool import(const diff& b);
    diff::hash_t get_last_block_imported() const;
    void set_last_block_imported(const diff::hash_t&);
    void set_last_block_imported_(const diff::hash_t&);
    vector<peer_t*> get_people();
    vector<peer_t*> get_nodes(const peer_t* exclude) const;
    const diff::hash_t& dbhash_off() const;
    string timestamp() const;

public:
    struct votes_t:unordered_map<pubkey_t::hash_t,pair<diff::hash_t,unsigned long>> { 
        typedef unordered_map<pubkey_t::hash_t,pair<diff::hash_t,unsigned long>> b;
        void clear();
        bool add(const pubkey_t::hash_t&,const diff::hash_t& v);
        diff::hash_t select();
        mutex mx;
    };

    struct sysops_t:unordered_map<peer_t *,shell> {
        sysops_t(daemon&d): d(d) {}
        bool process_work(peer_t *p, datagram*d);
        mutex mx;
        daemon& d;
    };

public:
    const keys& get_id() const {
        return m_id;
    } 

    mt19937_64& get_rng(){
        return m_rng;
    } 

    const networking& get_peerd() const {
        return m_peerd;
    }

    void add_ip_to_seed_nodes(const string& ip){
        m_peerd.add_ip_to_seed_nodes(ip); 
    }

    void peerd_daemon_timer(){
        m_peerd.daemon_timer();
    }

    auth::app* get_auth_app() const {
        return m_auth_app;
    }

    const string& get_home() const {
        return m_home;
    }

    const syncd_t& get_syncd() const {
        return m_syncd;
    }

private:
    hold<datagram> m_evidences_on_hold;
    cycle_t m_cycle;
    calendar_t m_calendar;
    diff::hash_t m_last_block_imported;
    mutable mutex m_mx_import;
    mutable diff::hash_t m_cached_dbhash;
    mutable bool m_cached_dbhash_ok{false};
    networking m_peerd;
    diff* m_pool{0};
    mutable mutex m_mx_pool;
    apps m_apps_;
    auth::app* m_auth_app;
    syncd_t m_syncd;
    sysops_t m_sysops;
    votes_t m_votes;
    string m_home;
    mutable mt19937_64 m_rng;
    keys m_id;
};
}}}

#endif

