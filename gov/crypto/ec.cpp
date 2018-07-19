#include "ec.h"
#include <sstream>
#include <iomanip>
#include <cassert>
#include "base58.h"

using namespace us::gov::crypto;
using namespace std;

typedef us::gov::crypto::ec c;

c c::instance;


static void secperr_callback(const char* str, void* data) {
    cerr << "libsecp256k1: " << str << endl;
    #ifdef DEBUG
    abort();
    #endif
}

c::ec() {
	ctx=secp256k1_context_create(SECP256K1_CONTEXT_SIGN|SECP256K1_CONTEXT_VERIFY); ///context to do EC operations
	if (ctx==0) {
		cerr << "Could not initialize EC context." << endl;
		exit(1);
	}
    secp256k1_context_set_illegal_callback(ctx, secperr_callback, 0);
}

c::~ec() {
	secp256k1_context_destroy(ctx);
}

c::keys::keys(const priv_t& pk) {
	memcpy(&priv[0],&pk[0],32);
//	for (int i=0; i<32; ++i) priv[i]=pk[i];
	if (!secp256k1_ec_pubkey_create(ec::instance.ctx,&pub,&pk[0])) {
		cerr << "Cannot create public key from priv" << endl;
		pub.zero();
	}
	else {
		pub.valid=true;
	}
//	assert(r==1);   <<-----------lint this querido markotas, ya me diras ma;ana como te lo has pasado de bien con Axel y Aida. ... - very well indeed.
	//assert(pub==get_pubkey(pk));

}

/*
c::keys::keys(const string& privk_b58,bool b) {
	priv=priv_t::from_b58(privk_b58);
/ *
	if (!verify(priv)) {
		cerr << "The private key is incorrect." << endl;
		exit(1);
	}
* /
	if (!secp256k1_ec_pubkey_create(ec::instance.ctx,&pub,&priv[0])) {
		cerr << "Invalid private key." << endl;
		priv.zero();
	}
}
*/

c::keys::keys(const keys& other): pub(other.pub) {
//	for (int i=0; i<32; ++i) priv[i]=other.priv[i];
	memcpy(&priv[0],&other.priv[0],32);
}

c::keys::keys(keys&& other): pub(other.pub) {
//	for (int i=0; i<32; ++i) priv[i]=other.priv[i];
	memcpy(&priv[0],&other.priv[0],32);
}

void c::keys::dump(ostream& os) const {
	os << "prv: " << to_hex(priv) << endl;
	os << "pub: " << pub << endl;
	os << endl;
}


c::keys::pub_t::hash_t c::keys::pub_t::compute_hash() const {
    if (unlikely(!valid)) return 0;
    unsigned char out[33];
    size_t len=33;
    if (unlikely(!secp256k1_ec_pubkey_serialize(ec::instance.ctx, out, &len, this, SECP256K1_EC_COMPRESSED))) {
	cerr << "cannot serialize pubkey" << endl;
	return 0;
    }

	hasher_t hasher;
//	hasher.write(&data[0],64); //only for LE platform TODO  
	hasher.write(&out[0],33); //only for LE platform TODO  
	hash_t v;
	hasher.finalize(v);
	return move(v);


}

string c::keys::to_hex(const priv_t& privkey) {
	ostringstream os;
	for (int i=0; i<32; ++i) os << hex << setfill('0') << setw(2) << (int)privkey[i]; //only LE //TODO
	return os.str();
}

bool c::keys::priv_t::is_zero() const {
    for (int i=0; i<32; i+=8)
        if (*reinterpret_cast<const uint64_t*>(&this[i])!=0) return false;
    return true;
}

string c::keys::priv_t::to_b58() const {
	return b58::encode(&*begin(),&*end());
}

string c::keys::pub_t::to_b58() const {
	if (unlikely(!valid)) return "";
	unsigned char out[33];
	size_t len=33;
	if (unlikely(!secp256k1_ec_pubkey_serialize(ec::instance.ctx, out, &len, this, SECP256K1_EC_COMPRESSED))) {
		cerr << "cannot serialize pubkey" << endl;
		return "";
	}
	auto s=b58::encode(out,out+len);
	return s;
}

string c::keys::pub_t::to_hex() const {
	if (unlikely(!valid)) return "";
	unsigned char out[33];
	size_t len=33;
	if (unlikely(!secp256k1_ec_pubkey_serialize(ec::instance.ctx, out, &len, this, SECP256K1_EC_COMPRESSED))) {
		cerr << "cannot serialize pubkey" << endl;
		return "";
	}
	ostringstream os;
	for (auto i=0; i<len; ++i) os << hex << setfill('0') << setw(2) << (int)out[i];
	return os.str();
}

c::keys::priv_t c::keys::priv_t::from_b58(const string& s) {
	priv_t k;
	if (!b58::decode(s.c_str(),k)) {
        k.zero();
//		cerr << "Error reading private key. " << s << endl;
	}
	return move(k);
}

bool c::keys::priv_t::set_b58(const string& s) {
	return b58::decode(s.c_str(),*this);
}

c::keys::pub_t c::keys::pub_t::from_b58(const string& s) {
	pub_t k;
	vector<unsigned char> v;
	v.reserve(32);
	if (!b58::decode(s,v)) {
		cerr << "Error reading public key, invalid b58 encoding. " << s << endl;
		k.zero();
		return move(k);
		//assert(false);
		//exit(1);//TODO not exit
	}
//	if (v.size()!=33) {
	if (v.empty()) {
		//cerr << "Error reading public key, invalid length." << endl;
		k.zero();
		return move(k);
		//assert(false);
		//exit(1);//TODO not exit
	}
	if (unlikely(!secp256k1_ec_pubkey_parse(ec::instance.ctx, &k, &v[0], v.size()))) {
		cerr << "Error reading public key." << endl;
		k.zero();
		return move(k);
	}
	k.valid=true;
	return move(k);
}

bool c::keys::pub_t::set_b58(const string& s) {
	vector<unsigned char> v;
	if (!b58::decode(s,v)) {
		//cerr << "Error reading public key, invalid b58 encoding." << endl;
		return false;
	}
	if (v.size()!=33) {
//		cerr << "Error reading public key, invalid length. " << v.size() << endl;
		return false;
	}
	if (unlikely(!secp256k1_ec_pubkey_parse(ec::instance.ctx, this, &v[0], 33))) {
		//cerr << "Error reading public key, invalid der encoding." << endl;
		valid=false;
		return false;
	}
	valid=true;
	return true;
//	if (r!=1) {
//		return false;
//	}
//	return true;
}

c::keys::pub_t c::keys::pub_t::from_hex(const string& s) {
	pub_t k;
	auto v=c::from_hex(s);
	if (v.size()!=33) {
		cerr << "Error reading public key, invalid length h." << endl;
		k.zero();
		return move(k);
//		assert(false);
//		exit(1);//TODO not exit
	}
	if (unlikely(!secp256k1_ec_pubkey_parse(ec::instance.ctx, &k, &v[0], 33))) {
		cerr << "Error reading public key." << endl;
		k.zero();
		return move(k);
//		assert(false);
//		exit(1);
	}
	k.valid=true;
	return move(k);
}
/*
bool c::keys::pub_t::assign(const string& s) {
	vector<unsigned char> v;
	if (!b58::decode(s,v)) {
		cerr << "Error reading public key, invalid b58 encoding." << endl;
		k.zero();
		return false;
//		assert(false);
//		exit(1);//TODO not exit
	}
	if (v.size()!=33) {
		cerr << "Error reading public key, invalid length." << endl;
		k.zero();
		return false;
//		exit(1);//TODO not exit
	}
	return secp256k1_ec_pubkey_parse(ec::instance.ctx, this, &v[0], 33)==1;
//	if (r!=1) {
//		cerr << "Error reading public key." << endl;
//		return false;
////		exit(1);
//	}
//	return true;
}
*/
bool c::keys::pub_t::operator == (const keys::pub_t& other) const {
	for (int i=0; i<64; i+=8)
	    if (*reinterpret_cast<const uint64_t*>(&data[i])!=*reinterpret_cast<const uint64_t*>(&other.data[i])) return false;
	return true;
}

c::keys::pub_t::pub_t(const pub_t& other):h(other.h), hash_cached(other.hash_cached), valid(other.valid)  {
	for (int i=0; i<64; i+=8)
	    *reinterpret_cast<uint64_t*>(&data[i])=*reinterpret_cast<const uint64_t*>(&other.data[i]);
}

c::keys::pub_t& c::keys::pub_t::operator = (const keys::pub_t& other) {
	for (int i=0; i<64; i+=8)
	    *reinterpret_cast<uint64_t*>(&data[i])=*reinterpret_cast<const uint64_t*>(&other.data[i]);
	valid=other.valid;
	hash_cached=other.hash_cached;
	h=other.h;
	return *this;
}

vector<unsigned char> c::sign(const keys::priv_t& pk, const string& text) const {
	sigmsg_hasher_t hasher;
	sigmsg_hasher_t::value_type hash;
	hasher.write((const unsigned char*)text.c_str(),text.size());
	hasher.finalize(hash);
	return sign(pk,hash);
}

vector<unsigned char> c::sign(const keys::priv_t& pk, const sigmsg_hasher_t::value_type& hash) const {
	vector<unsigned char> signature;
	signature.resize(72);
	size_t nSigLen=72;
	secp256k1_ecdsa_signature sig;
	if (unlikely(!secp256k1_ecdsa_sign(ctx, &sig, &hash[0], &pk[0], secp256k1_nonce_function_rfc6979, 0))) {
		cerr << "couldn't sign" << endl;
		return move(signature);
        }
//	assert(ret);
	if (unlikely(!secp256k1_ecdsa_signature_serialize_der(ctx, (unsigned char*)&signature[0], &nSigLen, &sig))) {
		cerr << "couldn't serialize signature" << endl;
		signature.clear();
		return move(signature);
	}
	signature.resize(nSigLen);
	return move(signature);
}

#include <iomanip>
#include <vector>

string c::to_hex(const vector<unsigned char>& data) {
	ostringstream os;
	for (auto i:data) os << hex << setfill('0') << setw(2) << (int)i;
	return os.str();
}
string c::to_b58(const vector<unsigned char>& data) {
	return b58::encode(&*data.begin(),(&*data.rbegin())+1);
}

string c::sign_encode(const keys::priv_t& pk, const string& text) const {
	return to_b58(sign(pk, text));
}

string c::sign_encode(const keys::priv_t& pk, const sigmsg_hasher_t::value_type& hash) const {
	return to_b58(sign(pk, hash));
}

bool c::verify(const keys::pub_t& pk, const sigmsg_hasher_t::value_type& msgh, const string& signature_der_b58) const {
	if (unlikely(signature_der_b58.empty())) return false;
	/// Parse the signature
	vector<unsigned char> sighex=c::from_b58(signature_der_b58);
	if (unlikely(sighex.empty())) return false;
	//cout << "verifying hash '" << msgh << "' " << sighex.size() << " " << sighex[0] << " " << signature_der_b58 << " " << pk << endl;
	signature sig;
	if (unlikely(!secp256k1_ecdsa_signature_parse_der(ctx,&sig,&sighex[0],sighex.size()))) {
		cerr << "cannot parse signature '" << signature_der_b58 << "' " << sighex.size() << endl;
		return false;
//		exit(1);
	}
	/// EC Verify
	return secp256k1_ecdsa_verify(ec::instance.ctx, &sig, &msgh[0], &pk)==1;
}

bool c::verify(const keys::pub_t& pk, const string& text, const string& signature_der_b58) const {
	//cout << "Verify: " << pk << ' ' << text << ' '<< signature_der_b58 << endl;
	sigmsg_hasher_t hasher;
	hasher.write((const unsigned char*)text.c_str(),text.size());
	sigmsg_hasher_t::value_type hash;
	hasher.finalize(hash);
	return verify(pk, hash, signature_der_b58);
}

bool c::keys::verify(const keys::priv_t& k) {
	return !k.is_zero() && secp256k1_ec_seckey_verify(ec::instance.ctx,&k[0])==1;
}

c::keys::pub_t c::keys::get_pubkey(const priv_t& privk) {
	pub_t k;
	if(unlikely(!secp256k1_ec_pubkey_create(ec::instance.ctx,&k,&privk[0]))) {
		cerr << "Error generating public key." << endl;
		k.zero();
		return k;
		//exit(1);
	}
	k.valid=true;
	return move(k);
}

#include <fstream>
c::keys c::keys::generate() {
	keys k;
	ifstream f("/dev/urandom");
	if (!f.good()) {
		cerr << "Error opening entropy file /dev/urandom" << endl;
		exit(1);
	}
	f.read(reinterpret_cast<char*>(&k.priv[0]),32);
	if (!verify(k.priv)) {
		cerr << "unexpected error generating privkey" << endl;
		exit(1);
	}
	k.pub=get_pubkey(k.priv);
	return move(k);
}

vector<unsigned char> c::from_hex(const string& hex) {
	vector<unsigned char> z;
	z.reserve(z.size()+hex.size()/2);
	for(int i=0; i<hex.size(); i+=2) {
		string byte = hex.substr(i,2);
		unsigned char chr = (unsigned char) (int)strtol(byte.c_str(), 0, 16);
		z.push_back(chr);
	}
	return move(z);
}

vector<unsigned char> c::from_b58(const string& b58) {
	vector<unsigned char> v;
	if (!b58::decode(b58,v)) {
		cerr << "Error reading b58. " << b58 << endl;
		v.clear();
		//assert(false);
		//exit(1);//TODO not exit
	}
	return move(v);
}

