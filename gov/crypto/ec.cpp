#include "ec.h"

#include <iomanip>
#include <iomanip>
#include <cassert>

#include <vector>
#include <fstream>
#include <sstream>

#include "base58.h"
#include <secp256k1_ecdh.h>

using namespace us::gov::crypto;
using namespace std;

static void secperr_callback(const char* str, void* data) {
    //TBD 
    cerr << "libsecp256k1: " << str << endl;
}

ec::ec() {
    m_ctx=secp256k1_context_create(SECP256K1_CONTEXT_SIGN|SECP256K1_CONTEXT_VERIFY); 
    if (m_ctx==0) {
        cerr << "Could not initialize EC context." << endl;
        exit(1);
    }
    secp256k1_context_set_illegal_callback(m_ctx, secperr_callback, 0);
}

ec::~ec() {
    secp256k1_context_destroy(m_ctx);
}

ec::keys::keys(const priv_t& pk) {
    memcpy(&m_priv[0],&pk[0],32);
    if (!secp256k1_ec_pubkey_create(ec::get_instance().m_ctx, &m_pub, &pk[0])) {
        cerr << "Cannot create public key from priv" << endl;
        m_pub.zero();
    }else{
        m_pub.set_valid(true);
    }
}

ec::keys::keys(const keys& other): m_pub(other.m_pub) {
    memcpy(&m_priv[0],&other.m_priv[0],32);
}

ec::keys::keys(keys&& other): m_pub(other.m_pub) {
    memcpy(&m_priv[0],&other.m_priv[0],32);
}

void ec::keys::dump(ostream& os) const {
    os << "prv: " << to_hex(m_priv) << endl;
    os << "pub: " << m_pub << endl;
    os << endl;
}

ec::keys::pub_t& ec::keys::pub_t:: operator =(const string& b58) {
    from_b58(b58);
    return *this;
}


ec::keys::pub_t::hash_t ec::keys::pub_t::compute_hash() const {
    if (unlikely(!m_valid)) 
        return 0;
    unsigned char out[33];
    size_t len=33;
    if (unlikely(!secp256k1_ec_pubkey_serialize(ec::get_instance().m_ctx, 
                    out, &len, this, SECP256K1_EC_COMPRESSED))) {
        cerr << "cannot serialize pubkey" << endl;
        return 0;
    }
    hasher_t hasher;
    hasher.write(&out[0],33); //only for LE platform TODO  
    hash_t v;
    hasher.finalize(v);
    return move(v);
}

string ec::keys::to_hex(const priv_t& privkey) {
    ostringstream os;
    for (int i=0; i<32; ++i){ 
        os << hex << setfill('0') << setw(2) 
                  << (int)privkey[i]; //only LE //TODO
    }
    return os.str();
}

bool ec::keys::priv_t::is_zero() const {
    for (int i=0; i<32; i+=8)
        if (*reinterpret_cast<const uint64_t*>(&this[i])!=0) return false;
    return true;
}

string ec::keys::priv_t::to_b58() const {
    return b58::encode(&*begin(),&*end());
}

string ec::keys::pub_t::to_b58() const {
    if (unlikely(!m_valid)) 
        return "";
    unsigned char out[33];
    size_t len=33;
    if (unlikely(!secp256k1_ec_pubkey_serialize(ec::get_instance().m_ctx, out, 
                                                &len, this, SECP256K1_EC_COMPRESSED))) {
        cerr << "cannot serialize pubkey" << endl;
        return "";
    }
    auto s=b58::encode(out,out+len);
    return s;
}

string ec::keys::pub_t::to_hex() const {
    if (unlikely(!m_valid)) 
        return "";
    unsigned char out[33];
    size_t len=33;
	if (unlikely(!secp256k1_ec_pubkey_serialize(ec::get_instance().m_ctx, out, 
                                                    &len, this, SECP256K1_EC_COMPRESSED))) {
        cerr << "cannot serialize pubkey" << endl;
        return "";
    }
    ostringstream os;
    for (auto i=0; i<len; ++i) 
        os << hex << setfill('0') << setw(2) << (int)out[i];
    return os.str();
}

ec::keys::priv_t ec::keys::priv_t::from_b58(const string& s) {
    priv_t k;
    if (!b58::decode(s.c_str(),k)) {
        k.zero();
    }
    return move(k);
}

bool ec::keys::priv_t::set_b58(const string& s) {
    return b58::decode(s.c_str(),*this);
}

ec::keys::pub_t ec::keys::pub_t::from_b58(const string& s) {
    pub_t k;
    vector<unsigned char> v;
    v.reserve(32);
    if (!b58::decode(s,v)) {
        cerr << "Error reading public key, invalid b58 encoding. " << s << endl;
        k.zero();
        return move(k);
        //TBD
    }
    if (v.empty()) {
        k.zero();
        return move(k);
    }
    if (unlikely(!secp256k1_ec_pubkey_parse(ec::get_instance().m_ctx, &k, &v[0], v.size()))) {
        cerr << "Error reading public key." << endl;
        k.zero();
        return move(k);
    }
    k.m_valid=true;
    return move(k);
}

bool ec::keys::pub_t::set_b58(const string& s) {
    vector<unsigned char> v;
    if (!b58::decode(s,v)) {
        return false;
    }
    if (v.size()!=33) {
        return false;
    }
    if (unlikely(!secp256k1_ec_pubkey_parse(ec::get_instance().m_ctx, this, &v[0], 33))) {
        m_valid=false;
        return false;
    }
    m_valid=true;
    return true;
}

ec::keys::pub_t ec::keys::pub_t::from_hex(const string& s) {
    pub_t k;
    auto v=ec::from_hex(s);
    if (v.size()!=33) {
        cerr << "Error reading public key, invalid length h." << endl;
        k.zero();
        return move(k);
    }
    if (unlikely(!secp256k1_ec_pubkey_parse(ec::get_instance().m_ctx, &k, &v[0], 33))) {
        cerr << "Error reading public key." << endl;
            k.zero();
            return move(k);
    }
    k.m_valid=true;
    return move(k);
}

bool ec::keys::pub_t::operator == (const keys::pub_t& other) const {
    for (int i=0; i<64; i+=8){
        if (*reinterpret_cast<const uint64_t*>(&data[i])!=*reinterpret_cast<const uint64_t*>(&other.data[i])) 
            return false;
    }
    return true;
}

ec::keys::pub_t::pub_t(const pub_t& other):
                        m_h(other.m_h), 
                        m_hash_cached(other.m_hash_cached), 
                        m_valid(other.m_valid)  {
    for (int i=0; i<64; i+=8)
        *reinterpret_cast<uint64_t*>(&data[i])=*reinterpret_cast<const uint64_t*>(&other.data[i]);
}

ec::keys::pub_t& ec::keys::pub_t::operator = (const keys::pub_t& other) {
    for (int i=0; i<64; i+=8)
        *reinterpret_cast<uint64_t*>(&data[i])=*reinterpret_cast<const uint64_t*>(&other.data[i]);
    m_valid=other.m_valid;
    m_hash_cached=other.m_hash_cached;
    m_h=other.m_h;
    return *this;
}

vector<unsigned char> ec::sign(const keys::priv_t& pk, const string& text) const {
    sigmsg_hasher_t hasher;
    sigmsg_hasher_t::value_type hash;
    hasher.write((const unsigned char*)text.c_str(),text.size());
    hasher.finalize(hash);
    return sign(pk,hash);
}

vector<unsigned char> ec::sign(const keys::priv_t& pk, 
                               const sigmsg_hasher_t::value_type& hash) const {
    vector<unsigned char> signature;
    signature.resize(72);
    size_t nSigLen=72;
    secp256k1_ecdsa_signature sig;
    if (unlikely(!secp256k1_ecdsa_sign(m_ctx, &sig, &hash[0], &pk[0], secp256k1_nonce_function_rfc6979, 0))) {
        cerr << "couldn't sign" << endl;
        return move(signature);
    }
    if (unlikely(!secp256k1_ecdsa_signature_serialize_der(m_ctx, 
                (unsigned char*)&signature[0], &nSigLen, &sig))) {
        cerr << "couldn't serialize signature" << endl;
        signature.clear();
        return move(signature);
    }
    signature.resize(nSigLen);
    return move(signature);
}

bool ec::generate_shared_key(unsigned char* shared_key, size_t size, 
                              const keys::priv_t& priv, const keys::pub_t& pub){
    unsigned char temp_key[32];
    if(!secp256k1_ecdh(m_ctx, temp_key, &pub, &priv[0], NULL, NULL)) {
        cerr << "Could not create shared secret";
        return false;
    }
    for(size_t i=0; i<size; i++){
        shared_key[i]=temp_key[i];
    }
    return true;
}

string ec::to_hex(const vector<unsigned char>& data) {
    ostringstream os;
    for (auto i:data) os << hex << setfill('0') << setw(2) << (int)i;
    return os.str();
}

string ec::to_b58(const vector<unsigned char>& data) {
    return b58::encode(&*data.begin(),(&*data.rbegin())+1);
}

string ec::sign_encode(const keys::priv_t& pk, const string& text) const {
    return to_b58(sign(pk, text));
}

string ec::sign_encode(const keys::priv_t& pk, 
                      const sigmsg_hasher_t::value_type& hash) const {
    return to_b58(sign(pk, hash));
}

bool ec::verify(const keys::pub_t& pk, 
               const string& text, 
               const string& signature_der_b58) const {
    sigmsg_hasher_t hasher;
    hasher.write((const unsigned char*)text.c_str(),text.size());
    sigmsg_hasher_t::value_type hash;
    hasher.finalize(hash);
    return verify(pk, hash, signature_der_b58);
}

bool ec::verify(const keys::pub_t& pk, 
               const sigmsg_hasher_t::value_type& msgh, 
               const string& signature_der_b58) const {
    if (unlikely(signature_der_b58.empty())) 
        return false;
    vector<unsigned char> sighex= ec::from_b58(signature_der_b58);
    if (unlikely(sighex.empty())) 
        return false;
    signature sig;
    if (unlikely(!secp256k1_ecdsa_signature_parse_der(
                        m_ctx,&sig,&sighex[0],sighex.size()))) {
        cerr << "cannot parse signature '"
             << signature_der_b58 << "' " << sighex.size() << endl;
        return false;
    }
    return secp256k1_ecdsa_verify(ec::get_instance().m_ctx, &sig, &msgh[0], &pk)==1;
}

bool ec::verify_not_normalized(const keys::pub_t& pk, 
                               const sigmsg_hasher_t::value_type& msgh, 
                               const string& signature_der_b58) const {
	
    if (unlikely(signature_der_b58.empty())) 
        return false;
    vector<unsigned char> sighex=ec::from_b58(signature_der_b58);
    if (unlikely(sighex.empty())) 
        return false;
    cout << "Input sig " << signature_der_b58 << endl;
    signature sig;
    if (unlikely(!secp256k1_ecdsa_signature_parse_der(
                        m_ctx,&sig,&sighex[0],sighex.size()))) {
        cerr << "cannot parse signature '" 
             << signature_der_b58 << "' " << sighex.size() << endl;
            return false;
    }
    signature nsig;
    secp256k1_ecdsa_signature_normalize(m_ctx,&nsig,&sig);
    return secp256k1_ecdsa_verify(ec::get_instance().m_ctx, &nsig, &msgh[0], &pk)==1;
}

bool ec::verify_not_normalized(const keys::pub_t& pk, 
                               const string& text, 
                               const string& signature_der_b58) const {
    sigmsg_hasher_t hasher;
    hasher.write((const unsigned char*)text.c_str(),text.size());
    sigmsg_hasher_t::value_type hash;
    hasher.finalize(hash);
    return verify_not_normalized(pk, hash, signature_der_b58);
}

bool ec::keys::verify(const keys::priv_t& k) {
    return !k.is_zero() && secp256k1_ec_seckey_verify(ec::get_instance().m_ctx,&k[0])==1;
}

ec::keys::pub_t ec::keys::get_pubkey(const priv_t& privk) {
    pub_t k;
    if(unlikely(!secp256k1_ec_pubkey_create(ec::get_instance().m_ctx,&k,&privk[0]))) {
        cerr << "Error generating public key." << endl;
        k.zero();
        return k;
    }
    k.set_valid(true);
    return move(k);
}

ec::keys ec::keys::generate() {
    keys k;
    ifstream f("/dev/urandom");
    if (!f.good()) {
        cerr << "Error opening entropy file /dev/urandom" << endl;
        exit(1);
    }
    f.read(reinterpret_cast<char*>(&k.m_priv[0]),32);
    if (!verify(k.m_priv)) {
        cerr << "unexpected error generating privkey" << endl;
        exit(1);
    }
    k.m_pub=get_pubkey(k.m_priv);
    return move(k);
}

vector<unsigned char> ec::from_hex(const string& hex) {
    vector<unsigned char> z;
    z.reserve(z.size()+hex.size()/2);
    for(int i=0; i<hex.size(); i+=2) {
        string byte = hex.substr(i,2);
        unsigned char chr = (unsigned char) (int)strtol(byte.c_str(), 0, 16);
        z.push_back(chr);
    }
    return move(z);
}

vector<unsigned char> ec::from_b58(const string& b58) {
    vector<unsigned char> v;
    if (!b58::decode(b58,v)) {
        cerr << "Error reading b58. " << b58 << endl;
        v.clear();
    }
    return move(v);
}

