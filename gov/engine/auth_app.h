#ifndef USGOV_1889feb46c109de0a04e33395e575f90dccd36322bcf52a8e6c93042928b612a
#define USGOV_1889feb46c109de0a04e33395e575f90dccd36322bcf52a8e6c93042928b612a

#include "app.h"
#include "peer_t.h"
#include "policies.h"
#include <string>
#include <us/gov/crypto/hash.h>
#include <map>
#include <set>
#include <mutex>
#include <random>
#include <unordered_set>

namespace us{ namespace gov{ namespace engine{

struct peer_t;

namespace auth { 
		
using namespace std;
using socket::datagram;

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
    constexpr static array<const char*,num_params> m_paramstr={"network_growth","network_min_growth"};
};

class app: public engine::runnable_app {
public:
    typedef engine::app b;
    app(const pubkey_t&);
    virtual ~app();

    struct local_delta: engine::policies_local_delta<double, policies_traits> {
        typedef engine::policies_local_delta<double, policies_traits> b;
        virtual ~local_delta() {}
        virtual int app_id() const override;
        virtual void to_stream(ostream& os) const override;
        virtual void from_stream(istream& is) override;
        vector<pair<pubkeyh_t,address>> m_to_hall; 
    };

    struct delta: engine::policies_delta<double, policies_traits, engine::average_merger<double>> {
        typedef engine::policies_delta<double, policies_traits, engine::average_merger<double>> b;
        delta() {}
        virtual ~delta() {}
        virtual uint64_t merge(engine::app::local_delta* other0) override {
            local_delta* other=static_cast<local_delta*>(other0);
            for (auto& i:other->m_to_hall) 
                m_to_hall.emplace(i);
            b::merge(other0);
            return 0;
            }
        virtual void to_stream(ostream& os) const override;
        static delta* from_stream(istream& is);
        map<pubkeyh_t,address> m_to_hall; 
    };

private:
    constexpr static const char* m_name={"auth"};

public:
    virtual string get_name() const override { 
        return m_name; 
    }

    static int id() { 
        return 20; 
    }

    virtual int get_id() const override { 
        return id(); 
    }

    virtual void run() override;
    virtual string shell_command(const string& cmdline) override;

    virtual void dbhash(engine::app::hasher_t&) const override;

    void basic_auth_completed(peer_t* p);

    void add_growth_transactions(unsigned int seed);
    void add_policies();

    peer_t::stage_t my_stage() const;

private:
    mutable peer_t::stage_t m_cache_my_stage{peer_t::unknown};

public:
    bool is_node() const { 
        return my_stage()==peer_t::node; 
    }

    virtual void import(const engine::app::delta&, const engine::pow_t&) override;

    virtual engine::app::local_delta* create_local_delta() override;
    virtual evidence* parse_evidence(uint16_t service, const string& datagram_payload) const override { 
        return 0;
    }
    virtual void process(const evidence&) override {}

private:
    local_delta* m_pool{0};
    mutex m_mx_pool;

public:
    string get_random_node(mt19937_64& rng, const unordered_set<string>& exclude_addrs) const; 

    struct db_t {
        db_t() {}

        peer_t::stage_t get_stage(const pubkeyh_t&) const;

        void hash(engine::app::hasher_t&) const;

        void dump(ostream& os) const;
        typedef unordered_map<pubkeyh_t,address> nodes_t; 
        typedef unordered_map<pubkeyh_t,address> hall_t; 

        void clear();
        
        mutable mutex m_mx_nodes;
        nodes_t m_nodes;

        mutable mutex m_mx_hall;
        hall_t m_hall;
    };

    const db_t& get_db()const{
        return m_db;
    }

private:		
    const pubkey_t& m_node_pubkey;
    db_t m_db;

public:
    virtual void clear() override;

    const keys& get_keys() const;

    struct policies_t: engine::policies_t<double, policies_traits> {
        typedef engine::policies_t<double, policies_traits> b;
        policies_t() {
            temp_load();
        }
	
        void temp_load() {
            (*this)[policies_traits::network_growth]=0.01; 
            (*this)[network_min_growth]=1.0;
        }
    };
    
    void dump_policies(ostream& os) const;

private:
    mutable mutex m_mx_policies;
    policies_t m_policies;
    policies_t m_policies_local;
};

}}}}
#endif

