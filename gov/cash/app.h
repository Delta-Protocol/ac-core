#ifndef US_GOV_CASH_APP_H
#define US_GOV_CASH_APP_H

#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <chrono>
#include <cassert>
#include <atomic>
#include <mutex>

#include <us/gov/auth.h>
#include <us/gov/engine.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/signal_handler.h>
#include <us/gov/crypto/crypto.h>
#include <us/gov/engine/diff.h>
#include <us/gov/engine/evidence.h>
#include "protocol.h"

namespace us{ namespace gov{ namespace cash{

using namespace std;
using socket::datagram;
using engine::local_deltas;
using crypto::ripemd160;
using crypto::ec;
using engine::peer_t;

typedef crypto::ec::keys keys;
typedef keys::pub_t pubkey_t;

typedef ripemd160 hasher_t;
typedef hasher_t::value_type hash_t;
typedef int64_t cash_t; 
typedef engine::evidence evidence;

static const hash_t min_locking_program(10000);

typedef uint32_t token_t;

class tx;

class app:public engine::runnable_app {
public:
    struct policies_traits {
       enum paramid {
            minimum_fee=0,
            num_params,
        };
        constexpr static array<const char*,num_params> m_paramstr={"minimum_fee"};
    };

    class local_delta: public engine::policies_local_delta<double, policies_traits> {
    public:       
        typedef engine::policies_local_delta<double, policies_traits> b;

        local_delta& operator =(int zero);
        local_delta();

        virtual ~local_delta();
        virtual int app_id() const override;
 
        virtual void to_stream(ostream&) const override;
        virtual void from_stream(istream&)     override;

    public:
        class tokens_t: public unordered_map<token_t,cash_t> {
        public:
            void dump(ostream& os)      const;
            void to_stream(ostream& os) const;
            static tokens_t from_stream(istream& is);
        };

        class safe_deposit_box {
        public: 
            //SC data goes here
            void dump(ostream& os)      const;
            void to_stream(ostream& os) const;
            static safe_deposit_box from_stream(istream& is);

            safe_deposit_box& operator = (const safe_deposit_box& other);
            safe_deposit_box& operator +=(const safe_deposit_box& other);
        private: 
            tokens_t m_balance;
        };

        typedef cash_t box_t;

        class account_t {
        public:
            account_t();
            account_t(const hash_t& locking_program, 
                      const box_t& balance         );

            void dump(ostream& os)      const;
            void to_stream(ostream& os) const;
            static account_t from_stream(istream& is);

            const hash_t& get_locking_program() const {
                return m_locking_program;
            }

            void set_locking_program(const hash_t&  lp){
                m_locking_program = lp;
            }

            const box_t& get_box() const {
                return m_box; 
            }
 
            void set_box(const box_t& box){
                m_box = box; 
            }   

        private:   
            hash_t m_locking_program;
            box_t m_box;
        };

        typedef account_t batch_item;
        
        class batch_t: public unordered_map<hash_t,batch_item> {
        public: 
            void add(const hash_t& address, const batch_item& bi);
        };

        class accounts_t: public map<hash_t,account_t> {
        public:    
            void add(const batch_t& batch);
            box_t get_balance() const;
            
            void to_stream(ostream& os) const;
            static accounts_t* from_stream(istream& is);

            void add_input(tx& t,  const hash_t& addr,
                                   const cash_t& amount);

            void add_output(tx& t, const hash_t& addr, 
                                   const cash_t& amount, 
                                   const hash_t& locking_program);
				
            void dump(ostream& os) const;
            bool pay(const hash_t& k, const box_t& amount);
            bool withdraw(const hash_t& k, const box_t& amount);
        };
    private:
        hash_t compute_hash()    const;
    public:       
        const hash_t& get_hash() const; 

        const accounts_t& get_accounts() const {
            return m_accounts; 
        }

        void set_accounts(const accounts_t& acc) {
            m_accounts = acc; 
        }

        const box_t& get_fees() const {
            return m_fees;
        }

        void set_fees(const box_t& fees){
            m_fees = fees;
        }
        
        void add(const batch_t& batch) {
            m_accounts.add(batch);    
        } 
    private: 
        accounts_t m_accounts; 
        box_t m_fees{0};
        mutable hash_t m_hash{0};        
    };

public:
    typedef local_delta::box_t box_t;

    class delta: public engine::policies_delta<double, policies_traits, engine::average_merger<double>> {
    public:    
        typedef engine::policies_delta<double, policies_traits, 
                                       engine::average_merger<double>> b;
        typedef local_delta::accounts_t accounts_t;
        
        delta();
        virtual ~delta();
        virtual uint64_t merge(engine::app::local_delta* other0) override;
        virtual void     end_merge()                             override;

        virtual void to_stream(ostream& os) const override;
        static delta* from_stream(istream& is);

        const local_delta& get_local_delta()const{
            return m_local_delta;
        }

        void local_delta_from_stream(istream& is){
            m_local_delta.from_stream(is);
        }
 
    private:
        local_delta m_local_delta;
        engine::majority_merger<local_delta>* m_m;
    };

    class query_accounts_t: public vector<hash_t> {
    public:
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

    typedef local_delta::account_t account_t;
    typedef local_delta::accounts_t accounts_t;

    class db_t {
    friend class app;  
    public: 
        db_t();
        db_t(db_t&& other);
        ~db_t();
    
    private: 
        bool add_(const hash_t&, const box_t& amount);
        bool withdraw_(const hash_t& k, const box_t& amount);

        void dump(ostream& os) const;
        cash_t get_newcash();
        void clear();

        void to_stream(ostream&) const;
        static db_t from_stream(istream&);  
            
    private:
        mutex& get_mx(){
            return m_mx;
        }

        const accounts_t* get_accounts() const {
            return m_accounts; 
        }    

    private:
        mutable mutex m_mx;
        accounts_t* m_accounts{0};
        cash_t m_supply_left;
        cash_t m_block_reward;
    };

    class policies_t: public engine::policies_t<double, policies_traits> {
    public:
        typedef engine::policies_t<double, policies_traits> b;
        policies_t() {
            temp_load();
        }
        void temp_load() {
            (*this)[minimum_fee]=1;
        }
    };

public:
    app();
    virtual ~app();

    virtual string get_name() const override { return m_name; }

    static bool unlock(const hash_t& address, 
                       const size_t& this_index, 
                       const hash_t& locking_program, 
                       const string& locking_program_input, const tx&);

    static int id() { return 30; }

private:
    virtual int get_id() const override { return id(); }

    virtual string shell_command(const string& cmdline) override;

    double supply_function(double x0, double x, double xf) const;

    void dbg_output() const;
    virtual void run() override;
    void add_policies();
    void dump_policies(ostream& os) const;

    virtual void dbhash(hasher_t&) const override;
    virtual void clear() override;

    virtual engine::app::local_delta* create_local_delta() override;

    virtual bool process_query(peer_t *, datagram*) override;

    virtual evidence* parse_evidence(uint16_t service,
                                     const string& datagram_payload) const override;
    bool process_tx(peer_t *, datagram*);
    void cash_query(peer_t *, datagram*);

    virtual void import(const engine::app::delta&, const engine::pow_t&) override;
    
    bool account_state(const local_delta::batch_t& batch, 
                       const hash_t& address, account_t& acc) const;

    bool checksig(const size_t& this_index, 
                  const string& locking_program_input, const tx&) const;

    virtual void process(const evidence&) override;
    bool basic_check(const tx&) const;

private:
    local_delta* m_pool{0};
    mutex m_mx_pool;
    constexpr static const char* m_name={"cash"};
    db_t db;
    mutable mutex m_mx_policies;
    policies_t m_policies;
    policies_t m_policies_local;
};

ostream& operator << (ostream&os, const app::local_delta::safe_deposit_box& b);
istream& operator >> (istream&os, const app::local_delta::safe_deposit_box& b);

}}}

#endif
