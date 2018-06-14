#include "single_signature.h"
#include <sstream>
#include <us/gov/crypto/crypto.h>


typedef us::gov::nova::single_signature c;
using namespace us::gov;
using namespace us::gov::nova;
using namespace std;

hash_t c::locking_program_hash{1};

bool c::check_input(const hash_t& compartiment, const evidence& e, const string& locking_program_input) {
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
		return false;
	}
	return true;
}
/*
string c::create_input(const tx& t, const size_t& this_index, const tx::sigcodes_t& sigcodes, const privkey_t& pk) {
	ec::sigmsg_hasher_t::value_type h=t.get_hash(this_index, sigcodes);
	return create_input(h,sigcodes,pk);
}

string c::create_input(const ec::sigmsg_hasher_t::value_type& h, const tx::sigcodes_t& sigcodes, const privkey_t& pk) {
	ostringstream os;
	os << ec::instance.sign_encode(pk,h) << ' ' << sigcodes << ' ' << keys::get_pubkey(pk);
	return os.str();
}
*/


