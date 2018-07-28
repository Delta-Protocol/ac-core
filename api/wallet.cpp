#include "wallet.h"
#include <sstream>
#include <us/gov/crypto.h>

typedef us::api::wallet c;
using namespace us::api;
using namespace std;

void c::priv_key(const gov::crypto::ec::keys::priv_t& privkey, ostream&os) {
	if (!gov::crypto::ec::keys::verify(privkey)) {
		os << "The private key is incorrect.";
        return;
	}
	auto pub=gov::crypto::ec::keys::get_pubkey(privkey);
	os << "Public key: " << pub << endl;
	os << "Address: " << pub.compute_hash();
}

void c::gen_keys(ostream&os) {
	gov::crypto::ec::keys k=gov::crypto::ec::keys::generate();
	os << "Private key: " << k.priv.to_b58() << endl;
	os << "Public key: " << k.pub.to_b58() << endl;
	os << "Address: " << k.pub.compute_hash();
}

