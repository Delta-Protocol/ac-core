#ifndef USGOV_075dfff39e95d4a4a7a088fe2ea0fc252d6fa47e6b62b043b4babd2e2b511780
#define USGOV_075dfff39e95d4a4a7a088fe2ea0fc252d6fa47e6b62b043b4babd2e2b511780

#include <iostream>
#include <vector>
#include <array>
#include <secp256k1.h>
#include "double_sha256.h"
#include <us/gov/likely.h>
#include "sha256.h"

namespace us{ namespace gov{ namespace crypto{
using namespace std;

struct ec {

typedef secp256k1_ecdsa_signature signature;
typedef sha256 sigmsg_hasher_t;

public:
    ec();
    ~ec();

    static ec& get_instance(){
        static ec instance;
        return instance;
    }    
    
    class keys {
    public:
        class pub_t: public secp256k1_pubkey {
        public:
            pub_t():m_valid(false), m_h(false) {}
            pub_t(const pub_t& other);
            typedef crypto::ripemd160 hasher_t;
            typedef hasher_t::value_type hash_t;

            inline const hash_t& hash() const { 
                if (!m_h) {
                    m_hash_cached=compute_hash();
                    m_h=true;
                }
                return m_hash_cached;
            }

            pub_t& operator =(const string&);
            hash_t compute_hash() const;
            string to_b58() const;
            string to_hex() const;
            bool set_b58(const string&);
            static pub_t from_b58(const string&);
            static pub_t from_hex(const string&);
			
            bool operator ==  (const pub_t& other) const;
            pub_t& operator = (const pub_t& other);

            inline void zero() { m_valid=false; }

            bool is_valid() const{
                return m_valid;
            }

            void set_valid(bool v){
                m_valid = v;
            } 

        private:
            bool m_valid{false};
            mutable bool m_h{false};
            mutable hash_t m_hash_cached;
        };

        class priv_t:public array<unsigned char,32> {
        public:  
            priv_t() {
                zero();
            }

            priv_t(const string& b58) {
                if (unlikely(!set_b58(b58))) 
                    zero();
            }

            priv_t(const char* b58) {
                if (unlikely(!set_b58(b58))) 
                    zero();
            }

            string to_b58() const;
            static priv_t from_b58(const string&);
            bool set_b58(const string&);

            inline void zero() { 
                memset(this,0,32); 
            }

            bool is_zero() const;
        };
	
    public:
        keys() {}
        keys(const priv_t&); 
        keys(const keys& other);
        keys(keys&& other);


        void dump(ostream& os) const;
        static string to_hex(const priv_t& privkey);

        static keys generate();

        static bool verify(const priv_t& privkey);
        static pub_t get_pubkey(const priv_t& privkey);

        const pub_t& get_pubkey()   const{
            return m_pub;
        }

        const priv_t& get_privkey() const{
            return m_priv;  
        }

    private:
        priv_t m_priv;
        pub_t  m_pub;
    };

    bool generate_shared_key(unsigned char *, size_t, 
                             const keys::priv_t&, 
                             const keys::pub_t&);

private:
    vector<unsigned char> sign(const keys::priv_t& pk, 
                               const string& text) const;
    vector<unsigned char> sign(const keys::priv_t& pk, 
                               const sigmsg_hasher_t::value_type&) const;

public:
    string sign_encode(const keys::priv_t& pk, 
                       const string& text) const;
    string sign_encode(const keys::priv_t& pk, 
                       const sigmsg_hasher_t::value_type&) const;

public:
    bool verify(const keys::pub_t& pk, 
                const string& text, 
                const string& signature_der) const;
    bool verify(const keys::pub_t& pk, 
                const sigmsg_hasher_t::value_type& msgh, 
                const string& signature_der_b58) const;
public:
    bool verify_not_normalized(const keys::pub_t& pk, 
                               const string& text, 
                               const string& signature_der) const;
    bool verify_not_normalized(const keys::pub_t& pk, 
                               const sigmsg_hasher_t::value_type& msgh, 
                               const string& signature_der_b58) const;

private:
    static constexpr size_t bit_size() { return 256; }

    static string to_hex(const vector<unsigned char>& data);
    static string to_b58(const vector<unsigned char>& data);
    static vector<unsigned char> from_hex(const string& hex);
    static vector<unsigned char> from_b58(const string& hex);

private:
    secp256k1_context* m_ctx;
};

inline ostream& operator << (ostream& os, const ec::keys::pub_t& k) {
    os << k.to_b58();
    return os;
}

inline istream& operator >> (istream& is, ec::keys::pub_t& k) {
    string s;
    is >> s;
    if (!k.set_b58(s)) {
        is.setstate(ios_base::failbit);
        k.zero();
    }
    return is;
}

inline ostream& operator << (ostream& os, const ec::keys::priv_t& k) {
    os << k.to_b58();
    return os;
}

inline istream& operator >> (istream& is, ec::keys::priv_t& k) {
    string s;
    is >> s;
    if (!k.set_b58(s)) {
        is.setstate(ios_base::failbit);
        k.zero();
    }
    return is;
}

}}}

#endif
