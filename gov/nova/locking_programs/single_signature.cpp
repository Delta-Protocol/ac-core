#include "single_signature.h"
#include <sstream>
#include <us/gov/crypto/crypto.h>


typedef us::gov::nova::single_signature c;
using namespace us::gov;
using namespace us::gov::nova;
using namespace std;

hash_t c::locking_program_hash{1};

bool c::check_input(const hash_t& compartiment, const evidence& e, const string& locking_program_input) {
cout << "check input: " << compartiment << " " << e.get_hash() << " " << locking_program_input << endl;
	istringstream is(locking_program_input);
	keys::pub_t pk;
	string signature_der_b58;
	is >> signature_der_b58;
	is >> pk;
	if (pk.hash()!=compartiment) {
		cout << "compartiment pubkey check failed." << endl;
		return false;
	}

	ec::sigmsg_hasher_t::value_type h=e.get_hash();

	if (!ec::instance.verify(pk, h, signature_der_b58)) {
cout << "verify failed: " << pk << " " << h << " " << signature_der_b58 << endl;
		return false;
	}
	return true;
}

string c::create_input(const evidence& t, const privkey_t& pk) {
	ec::sigmsg_hasher_t::value_type h=t.get_hash();
cout << "XXX " << t.compartiment << " ==? " << keys::get_pubkey(pk) << endl;
	return create_input(h,pk);
}

string c::create_input(const ec::sigmsg_hasher_t::value_type& h, const privkey_t& pk) {
	ostringstream os;
cout << "signing with privk " << pk << " pubk " << keys::get_pubkey(pk) << " hashmsg " << h << " signature " << ec::instance.sign_encode(pk,h) << endl;
	os << ec::instance.sign_encode(pk,h) << ' ' << keys::get_pubkey(pk);

    string input=os.str();
    cout << "input: " << input << endl;

	return os.str();
}



