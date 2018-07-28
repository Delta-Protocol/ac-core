#include "wallet.h"
#include "protocol.h"
#include <sstream>
#include <us/gov/cash.h>

typedef us::wallet::wallet::api c;
using namespace us::wallet;
using namespace std;

void c::priv_key(const crypto::ec::keys::priv_t& privkey, ostream&os) {
	if (!crypto::ec::keys::verify(privkey)) {
		os << "The private key is incorrect.";
        return;
	}
	auto pub=crypto::ec::keys::get_pubkey(privkey);
	os << "Public key: " << pub << endl;
	os << "Address: " << pub.compute_hash();
}

void c::gen_keys(ostream&os) {
	crypto::ec::keys k=crypto::ec::keys::generate();
	os << "Private key: " << k.priv.to_b58() << endl;
	os << "Public key: " << k.pub.to_b58() << endl;
	os << "Address: " << k.pub.compute_hash();
}

