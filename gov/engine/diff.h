#ifndef US_GOV_ENGINE_DIFF_H
#define US_GOV_ENGINE_DIFF_H

#include <vector>
#include <cassert>
#include <unordered_map>
#include <thread>
#include <map>

#include "app.h"
#include "signed_data.h"

#include <us/gov/socket.h>
#include <us/gov/auth.h>
#include <us/gov/crypto/hash.h>
#include <us/gov/crypto/ec.h>

namespace us{ namespace gov{ namespace engine{

using namespace std;
using socket::datagram;
using crypto::ripemd160;

typedef crypto::ec::keys keys;
typedef keys::pub_t pubkey_t;
typedef pubkey_t::hash_t pubkeyh_t;

struct peer_t;
struct diff;

struct local_deltas: map<int,app::local_delta*>, signed_data {
    virtual ~local_deltas() {}
    string message_to_sign() const override;
    void to_stream(ostream&) const;
    static local_deltas* from_stream(istream&);
};

struct pow_t:map<pubkey_t::hash_t, uint64_t> {
    uint64_t sum() const {
        uint64_t s=0;
        for (auto&i:*this) {
            s+=i.second;
        }
        return s;
    }
};

class diff: public map<int,app::delta*> {
public:
    typedef map<int,app::delta*> b;
    typedef app::hasher_t hasher_t;
    typedef app::hash_t hash_t;

    diff() {}
    diff(const diff&)=delete;

    virtual ~diff() {
        for (auto& i:*this) delete i.second;
    }
   
public:
    // only one local_deltas per pubkey is allowed to be added
    bool allow(const local_deltas&);     
    //returns false when local_deltas exists for this pubk
    void add(local_deltas*); 
    void end_adding();

    const hash_t& hash() const;

    static diff* from_stream(istream&);
    string parse_string() const;

    const hash_t& get_previews_hash() const {
        return m_prev;
    }

    void set_previews_hash(const hash_t& h){
        m_prev = m_h;
    }

    const pow_t& get_proof_of_work() const {
        return m_proof_of_work;
    }

private:
    uint64_t add(int appid, app::local_delta* g);
    void to_stream(ostream&) const;
    static hash_t hash(const string&);

private:
    mutable hash_t m_hash_cached;
    mutable bool m_h{false}; 
    hash_t m_prev; 
    pair<hash_t,hash_t> m_base; /// (5 cycles ago block hash, base hash) (5 diff ago can be applied to this base)
    mutex m_mx;
    pow_t m_proof_of_work;
    mutex m_mx_proof_of_work;
};

}}}

#endif
