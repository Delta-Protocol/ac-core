#ifndef US_GOV_ENGINE_SIGNED_DATA_H
#define US_GOV_ENGINE_SIGNED_DATA_H

#include <string>

#include <us/gov/crypto/hash.h>
#include <us/gov/crypto/ec.h>

namespace us{ namespace gov{ namespace engine{
using namespace std;
using crypto::ripemd160;
typedef crypto::ec::keys keys;
typedef keys::pub_t pubkey_t;
typedef pubkey_t::hash_t pubkeyh_t;

class signed_data {
public:
    virtual ~signed_data() {}
    void sign(const crypto::ec::keys&);
    bool verify() const;

public:
    const pubkey_t& get_pubkey() const {
        return m_pubkey;
    }
 
    const string& get_signature() const {
        return m_signature;
    } 

protected:
    virtual string message_to_sign() const=0;

    void set_pubkey(const pubkey_t& k){
        m_pubkey = k;
    }

    void set_signature(const string& s) {
        m_signature = s;
    } 

private:
    pubkey_t m_pubkey; 
    string m_signature; 
};

}}}

#endif

