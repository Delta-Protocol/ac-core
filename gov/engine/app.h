#ifndef US_GOV_ENGINE_APP_H
#define US_GOV_ENGINE_APP_H

#include <vector>
#include <cassert>
#include <thread>
#include <chrono>
#include <mutex>

#include <us/gov/peer.h>
#include <us/gov/socket.h>
#include "evidence.h"
#include <us/gov/crypto.h>


namespace us{ namespace gov{ namespace engine{
using namespace std;
using socket::datagram;

struct peer_t;
class local_delta;
class delta;
struct block;
struct pow_t;

struct daemon;
struct block;

class app {
public:
typedef crypto::ripemd160 hasher_t;
typedef hasher_t::value_type hash_t;

    class local_delta {
    public:
        virtual ~local_delta() {}

        static local_delta* create(int id);
        static local_delta* create(istream&);
        static local_delta* create(int id, istream& is);

        virtual void to_stream(ostream&) const=0;
        virtual void from_stream(istream&)=0;
        virtual int app_id() const=0;
    };

    class delta {
    public:
        delta() {}
        virtual ~delta() {}

        static delta* create(int id);
        static delta* create(istream&);
        static delta* create(int id,istream&);

        virtual uint64_t merge(local_delta* other);
        virtual void end_merge()=0;
        virtual void to_stream(ostream&) const=0;
        unsigned long get_multiplicity() const {
            return m_multiplicity;
        } 
    private:
        unsigned long m_multiplicity{0};
    };

public:
    app() {}
    virtual ~app() {}

    virtual int get_id() const=0;

    //provide a name for your app
    virtual string get_name() const=0; 
    
    //override and implement a db reset to a genesis state.
    virtual void clear()=0;
    
    //override and return the hash of the content of your db
    virtual void dbhash(hasher_t&) const=0; 

    //override and apply the given delta to your db
    virtual void import(const delta&, const pow_t&)=0; 
    
    virtual evidence* parse_evidence(uint16_t service, const string& datagram_payload) const = 0;
    virtual void process(const evidence&) = 0;

    //override to respond to user's read-only queries
    virtual bool process_query(peer_t *, datagram*) { 
        return false; 
    } 

    // override to be able to submit your diff (your mempool)
    virtual local_delta* create_local_delta()=0; 

     //answers to shell commands
    virtual string shell_command(const string& cmdline);

    static unsigned int get_seed();

public:
    class chaininfo_t {
    public:
        chaininfo_t():m_imported_last_block(0), m_tip(0) {}
        bool not_equals_tip(const hash_t& h) const {
            unique_lock<mutex> lock(m_mx_tip);
            return m_tip!=h;
        }
        const hash_t& get_tip() const {
            unique_lock<mutex> lock(m_mx_tip);
            return m_tip;
        }
        void set_tip(const hash_t& t) {
            unique_lock<mutex> lock(m_mx_tip);
            m_tip=t;
        }

        const hash_t& get_imported_last_block() const{
            return m_imported_last_block; 
        }

        void set_imported_last_block(const hash_t b){
            m_imported_last_block = b;
        }
    private:
        hash_t m_imported_last_block; 
        hash_t m_tip; 
        mutable mutex m_mx_tip;
    };

public:
    static chaininfo_t& get_chain_info(){
        static chaininfo_t chaininfo;
        return chaininfo; 
    } 
};

class runnable_app:public app {
public:
    virtual void run()=0;
};

template<typename D, typename T>
class policies_base: public array<D,T::num_params> {
public:
typedef array<D,T::num_params> base;
    policies_base() {
        for (auto&i:*this) 
            i=0;
    }

    policies_base(const base& g):base(g) {}
    virtual ~policies_base() {}

    void to_stream(ostream& os) const {
        for (auto& i:*this) {
            os << i << ' ';
        }
    }

    void from_stream(istream& is) {
        for (int i=0; i<T::num_params; ++i) {
            double v;
            is >> v;
            (*this)[i]=v;
        }
    }
};

template<typename D, typename T>
class policies_local_delta:public policies_base<D,T>, public app::local_delta {
public:
typedef policies_base<D,T> base_1;
typedef local_delta base_2;
    policies_local_delta() {}
    policies_local_delta(const typename base_1::base& g): base_1(g) {}
    virtual ~policies_local_delta() {}
    virtual void to_stream(ostream& os) const override {
        base_1::to_stream(os);
    }
    void from_stream(istream& is) {
        base_1::from_stream(is);
    }
};

template<typename D>
class average_merger {
public:
    void merge(D& v1, const D& v2) const {
        v1+=v2;
    }
    void end_merge(D& v, const unsigned long& multiplicity) const {
        double m=multiplicity;
        v/=m;
    }
};

template<typename D>
class majority_merger {
public:
    void merge(D&, const D& v2) {
        auto i=m.find(v2);
        if (i!=m.end()) {
            ++i->second;
            return;
        }
        m.emplace(v2,1);
    }

    void end_merge(D& v, const unsigned long&) {
        const D* x=0;
        unsigned long z=0;
        for (auto& i:m) {
            if (i.second>z) {
                z=i.second;
                x=&i.first;
            }
        }
        if (likely(z!=0)) 
            v=*x;
        else v=0;
            m.clear();
    }

    unordered_map<D,unsigned long> m;
};

template<typename D, typename T, typename M>
class policies_delta:public policies_base<D,T>, public app::delta {
public:
    typedef policies_base<D,T> base_1;
    typedef delta base_2;
    policies_delta() {}
    policies_delta(const policies_local_delta<D,T>& g): base_1(g) {}
    virtual ~policies_delta() {}

    virtual uint64_t merge(app::local_delta* other0) override {
        base_1* other=dynamic_cast<base_1*>(other0); 
        for (int i=0; i<T::num_params; ++i) 
            m_merger.merge((*this)[i],(*other)[i]);  
        base_2::merge(other0);
        return 0;
    }

    virtual void end_merge() override {
        double m=base_2::get_multiplicity();
        for (int i=0; i<T::num_params; ++i) 
            m_merger.end_merge((*this)[i],m);
    }

private:
    M m_merger;
};

template<typename D, typename T>
class policies_t:public array<D,T::num_params>, public T {
public:
typedef array<D,T::num_params> base_1;
typedef app::hasher_t hasher_t;

    policies_t() {
        clear();
    }

    virtual ~policies_t() {}

    void clear() {
        for (auto& i:*this) 
            i=0;
    }

    policies_t& operator =(const policies_t& other) {
        for (int i=0; i<T::num_params; ++i) {
            (*this)[i]=other[i];
        }
        return *this;
    }

    base_1 operator -(const policies_t& other) const {
        base_1 ans;
        for (int i=0; i<T::num_params; ++i) {
            ans[i]=(*this)[i]-other[i];
        }
        return move(ans);
    }

    void hash(hasher_t& h) const {
        for (auto& i:*this) 
            h << i;
    }
};

}}}
#endif

