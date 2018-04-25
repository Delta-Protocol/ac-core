#include "ec.h"
#include <sstream>
#include <iomanip>
#include <cassert>
#include "base58.h"

using namespace usgov::crypto;
using namespace std;

typedef usgov::crypto::ec c;

c c::instance;

c::ec() {
	ctx=secp256k1_context_create(SECP256K1_CONTEXT_SIGN|SECP256K1_CONTEXT_VERIFY); ///context to do EC operations
}

c::~ec() {
	secp256k1_context_destroy(ctx);
}

c::keys::keys(const priv_t& pk) {
	for (int i=0; i<32; ++i) priv[i]=pk[i];
	int r=secp256k1_ec_pubkey_create(ec::instance.ctx,&pub,&pk[0]);
	assert(r==1); 
	//assert(pub==get_pubkey(pk));

}

c::keys::keys(const string& privk_b58) {
	priv=priv_t::from_b58(privk_b58);
	if (!verify(priv)) {
		cerr << "The private key is incorrect." << endl;
		exit(1);
	}
	int r=secp256k1_ec_pubkey_create(ec::instance.ctx,&pub,&priv[0]);
	if(r!=1) {
		cerr << "Invalid private key." << endl;
		exit(1);
	}
}

c::keys::keys(const keys& other): pub(other.pub) {
	for (int i=0; i<32; ++i) priv[i]=other.priv[i];
}

c::keys::keys(keys&& other): pub(other.pub) {
	for (int i=0; i<32; ++i) priv[i]=other.priv[i];
}

void c::keys::dump(ostream& os) const {
	os << "prv: " << to_hex(priv) << endl;
	os << "pub: " << pub << endl;
	os << endl;
}

c::keys::pub_t::hash_t c::keys::pub_t::compute_hash() const {
	hasher_t hasher;
	hasher.write(&data[0],64);
	hash_t v;
	hasher.finalize(v);
	return move(v);
}

string c::keys::to_hex(const priv_t& privkey) {
	ostringstream os;
	for (int i=0; i<32; ++i) os << hex << setfill('0') << setw(2) << (int)privkey[i];
	return os.str();
}

string c::keys::priv_t::to_b58() const {
	return b58::encode(&*begin(),&*end());
}

string c::keys::pub_t::to_b58() const {
	unsigned char out[33];
	size_t len=33;
	secp256k1_ec_pubkey_serialize(ec::instance.ctx, out, &len, this, SECP256K1_EC_COMPRESSED);
	auto s=b58::encode(out,out+len);
	return s;
}

string c::keys::pub_t::to_hex() const {
	unsigned char out[33];
	size_t len=33;
	secp256k1_ec_pubkey_serialize(ec::instance.ctx, out, &len, this, SECP256K1_EC_COMPRESSED);
	ostringstream os;
	for (auto i=0; i<len; ++i) os << hex << setfill('0') << setw(2) << (int)out[i];
	return os.str();
}

c::keys::priv_t c::keys::priv_t::from_b58(const string& s) {
	priv_t k;
	if (!b58::decode(s.c_str(),k)) {
		cerr << "Error reading private key. " << s << endl;
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
		assert(false);
		exit(1);//TODO not exit
	}
	if (v.size()!=33) {
		cerr << "Error reading public key, invalid length." << endl;
		assert(false);
		exit(1);//TODO not exit
	}
	auto r=secp256k1_ec_pubkey_parse(ec::instance.ctx, &k, &v[0], 33);
	if (r!=1) {
		cerr << "Error reading public key." << endl;
		assert(false);
		exit(1);
	}
	return move(k);
}

bool c::keys::pub_t::set_b58(const string& s) {
	vector<unsigned char> v;
	if (!b58::decode(s,v)) {
		return false;
	}
	if (v.size()!=33) {
		return false;
	}
	auto r=secp256k1_ec_pubkey_parse(ec::instance.ctx, this, &v[0], 33);
	if (r!=1) {
		return false;
	}
	return true;
}

c::keys::pub_t c::keys::pub_t::from_hex(const string& s) {
	pub_t k;
	auto v=c::from_hex(s);
	if (v.size()!=33) {
		cerr << "Error reading public key, invalid length." << endl;
		assert(false);
		exit(1);//TODO not exit
	}
	auto r=secp256k1_ec_pubkey_parse(ec::instance.ctx, &k, &v[0], 33);
	if (r!=1) {
		cerr << "Error reading public key." << endl;
		assert(false);
		exit(1);
	}
	return move(k);
}

void c::keys::pub_t::assign(const string& s) {
	vector<unsigned char> v;
	if (!b58::decode(s,v)) {
		cerr << "Error reading public key, invalid b58 encoding." << endl;
		assert(false);
		exit(1);//TODO not exit
	}
	if (v.size()!=33) {
		cerr << "Error reading public key, invalid length." << endl;
		exit(1);//TODO not exit
	}
	auto r=secp256k1_ec_pubkey_parse(ec::instance.ctx, this, &v[0], 33);
	if (r!=1) {
		cerr << "Error reading public key." << endl;
		exit(1);
	}
}

bool c::keys::pub_t::operator == (const keys::pub_t& other) const {
	for (int i=0; i<64; i+=8)
	    if (*reinterpret_cast<const uint64_t*>(&data[i])!=*reinterpret_cast<const uint64_t*>(&other.data[i])) return false;
	return true;
}

c::keys::pub_t& c::keys::pub_t::operator = (const keys::pub_t& other) {
	for (int i=0; i<64; i+=8)
	    *reinterpret_cast<uint64_t*>(&data[i])=*reinterpret_cast<const uint64_t*>(&other.data[i]);
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
	int ret = secp256k1_ecdsa_sign(ctx, &sig, &hash[0], &pk[0], secp256k1_nonce_function_rfc6979, 0);
	assert(ret);
	secp256k1_ecdsa_signature_serialize_der(ctx, (unsigned char*)&signature[0], &nSigLen, &sig);
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
	int rt=secp256k1_ecdsa_signature_parse_der(ctx,&sig,&sighex[0],sighex.size());
	if (unlikely(rt!=1)) {
		cerr << "cannot parse signature '" << signature_der_b58 << "' " << sighex.size() << endl;
		exit(1);
	}
	/// EC Verify
        int ret = secp256k1_ecdsa_verify(ec::instance.ctx, &sig, &msgh[0], &pk);
	return ret==1;
}

bool c::verify(const keys::pub_t& pk, const string& text, const string& signature_der_b58) const {
	//cout << "Verify: " << pk << ' ' << text << ' '<< signature_der_b58 << endl;
	sigmsg_hasher_t hasher;
	hasher.write((const unsigned char*)text.c_str(),text.size());
	sigmsg_hasher_t::value_type hash;
	hasher.finalize(hash);
	return verify(pk, hash, signature_der_b58);
}

bool c::keys::verify(const keys::priv_t& privkey) {
	int r=secp256k1_ec_seckey_verify(ec::instance.ctx,&privkey[0]);
	return r==1;
}

c::keys::pub_t c::keys::get_pubkey(const priv_t& privk) {
	pub_t k;
	int r=secp256k1_ec_pubkey_create(ec::instance.ctx,&k,&privk[0]);
	if(r!=1) {
		cerr << "Error generating public key." << endl;
		exit(1);
	}
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
		assert(false);
		exit(1);//TODO not exit
	}
	return move(v);
}

