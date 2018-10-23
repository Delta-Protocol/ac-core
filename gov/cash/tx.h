#ifndef USGOV_33de4fece6646b4a064a30507e7a1a35c3b5bcd4dc9c1c87059904e7ac05e335
#define USGOV_33de4fece6646b4a064a30507e7a1a35c3b5bcd4dc9c1c87059904e7ac05e335

#include <vector>
#include <map>
#include <unordered_map>
#include <unordered_set>

#include <thread>
#include <chrono>
#include <cassert>
#include <memory>

#include <us/gov/auth.h>
#include <us/gov/engine.h>
#include <us/gov/signal_handler.h>
#include <us/gov/crypto/crypto.h>

#include "protocol.h"
#include "app.h"

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
    
class tx:public engine::evidence {
public:
    enum sigcode_t {
        sigcode_all  = 0,
        sigcode_none = 1,
        sigcode_this = 2,
        num_sigcodes
    };

    class input_t {
    public:
        input_t() {}
        input_t(const hash_t& address, 
                const cash_t& amount): 
                        m_address(address), 
                        m_amount(amount)  {
        }
        input_t(const hash_t& address, 
                const cash_t& amount, 
                const string& locking_program_input): 
                        m_address(address), m_amount(amount), 
                        m_locking_program_input(locking_program_input) {
        }
        
        void write_sigmsg(ec::sigmsg_hasher_t&) const;
        void write(ostream&)                    const;
        void write_pretty(ostream& os)          const;
        
        static pair<string,input_t> read(istream&);

    public:
        const hash_t& get_address() const {
            return m_address;
        }  

        const cash_t& get_amount() const {
            return m_amount;
        }  

        const string& get_locking_program_input() const {
            return m_locking_program_input;
        } 
 
        void set_locking_program_input(const string& input){
            m_locking_program_input = input;
        } 

    private:
        hash_t m_address;
        cash_t m_amount;
        string m_locking_program_input;
    };

    class output_t {
    public:
        output_t() {}
        output_t(const hash_t& address, 
                  const cash_t& amount, 
                  const hash_t& locking_program): 
                          m_address(address), m_amount(amount),
                          m_locking_program(locking_program) {
        }
         
        void write_sigmsg(ec::sigmsg_hasher_t&) const;
        void write(ostream&)                    const;
        void write_pretty(ostream& os)          const;

        static pair<string,output_t> read(istream&);

    public:
        const hash_t& get_address() const {
            return m_address;
        }  

        const cash_t& get_amount() const {
            return m_amount;
        }  

        const hash_t& get_locking_program() const {
            return m_locking_program;
        } 
 
        void set_locking_program(const hash_t& p){
            m_locking_program = p;
        } 
    private:
        hash_t m_address;
        cash_t m_amount;
        hash_t m_locking_program;
    };

    typedef unsigned char sigcodes_t;

    struct inputs_t:vector<input_t> {
        void write_sigmsg(ec::sigmsg_hasher_t&, 
                          size_t input_index, 
                          sigcode_t sh) const;
        void write(ostream&)            const;
        void write_pretty(ostream& os)  const;
        string read(istream&);
    };

    struct outputs_t:vector<output_t> {
        void write_sigmsg(ec::sigmsg_hasher_t&, 
                          size_t input_index, 
                          sigcode_t sh) const;
        void write(ostream&)            const;
        void write_pretty(ostream& os)  const;
        string read(istream&);
    };

public:
    tx(){}
    tx(int){}

    virtual ~tx() {}

    static void dump_sigcodes(ostream&os);
    cash_t check() const; 

    ec::sigmsg_hasher_t::value_type get_hash(const size_t& this_index,
                                                           sigcodes_t) const;
 
public:
    static pair<string,unique_ptr<tx>> read(istream&);

    void write(ostream&)                   const;
    void write_sigmsg(ec::sigmsg_hasher_t&, 
                      size_t input_index, 
                      sigcodes_t sh)       const;
    void write_pretty(ostream& os)         const;

public:
    string to_b58() const;

    static pair<string,unique_ptr<tx>> from_b58(const string&);
    
    datagram* get_datagram() const;

public:
    void add_input( const hash_t& addr, 
                    const cash_t& amount);
    void add_input( const hash_t& addr, 
                    const cash_t& amount, 
                    const string& locking_program_input);
    void add_output(const hash_t& addr, 
                    const cash_t& amount, 
                    const hash_t& locking_program);

public:
    static inline sigcodes_t combine(sigcode_t i, sigcode_t o) { 
        return (i<<4)|o; 
    }
    static inline sigcode_t sigcode_input(sigcodes_t i) { 
        return (sigcode_t)(i>>4); 
    }
    static inline sigcode_t sigcode_output(sigcodes_t i){ 
        return (sigcode_t)(i&0x0f); 
    }
    static bool same_sigmsg_across_inputs(sigcodes_t i) { 
        return sigcode_input(i)==sigcode_this || 
                sigcode_output(i)==sigcode_this; 
    }

public:

    static const array<const char*,num_sigcodes>& get_sigcodestr(){
        return m_sigcodestr;
    }

private:
    constexpr static array<const char*,num_sigcodes> 
                         m_sigcodestr={"all","none","this"};

public:
    const inputs_t& get_inputs(){
        return m_inputs;
    }

    const outputs_t& get_outputs(){
        return m_outputs;
    }
    
    void set_locking_program_input(int n, const string& s){
        if(m_inputs.size() < n){
            m_inputs[n].set_locking_program_input(s);
        }else{
            assert(!"Out of boundary");
        }
    }

private:
    inputs_t m_inputs;
    outputs_t m_outputs;
};


static ostream& operator << (ostream&os, const cash::tx& t)             {
    os << t.to_b58();
    return os;
}

static ostream& operator << (ostream&os, const cash::tx::sigcodes_t& x) {
    os << (int)x;
    return os;
}

static istream& operator >> (istream&is, cash::tx::sigcodes_t& t)       {
    int x;
    is >> x;
    t=(cash::tx::sigcodes_t)x;
    return is;
}

static ostream& operator << (ostream&os, const cash::tx::sigcode_t& x)  {
    os << (int)x;
    return os;
}

static istream& operator >> (istream&is, cash::tx::sigcode_t& t)        {
    int x;
    is >> x;
    t=(cash::tx::sigcode_t)x;
    return is;
}

}}}

#endif

