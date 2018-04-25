#include "p2pkh.h"
#include <sstream>
#include <gov/crypto/crypto.h>


typedef usgov::cash::p2pkh c;
using namespace usgov;
using namespace usgov::cash;
using namespace std;

hash_t c::locking_program_hash{1};

bool c::check_input(const hash_t& addreess, const tx& t, const size_t& this_index, const string& locking_program_input) {
	istringstream is(locking_program_input);
	keys::pub_t pk;
	string signature_der_b58;
	tx::sigcodes_t sigcodes;
	is >> signature_der_b58;
	is >> sigcodes;
	is >> pk;
	if (pk.hash()!=addreess) {
		cout << "Address check failed." << endl;
		return false;
	}

	ec::sigmsg_hasher_t::value_type h=t.get_hash(this_index, sigcodes);

	if (!ec::instance.verify(pk, h, signature_der_b58)) {
		return false;
	}
	return true;
}

string c::create_input(const tx& t, const size_t& this_index, const tx::sigcodes_t& sigcodes, const privkey_t& pk) {
	ec::sigmsg_hasher_t::value_type h=t.get_hash(this_index, sigcodes);
	return create_input(h,sigcodes,pk);
}

string c::create_input(const ec::sigmsg_hasher_t::value_type& h, const tx::sigcodes_t& sigcodes, const privkey_t& pk) {
	ostringstream os;
	os << ec::instance.sign_encode(pk,h) << ' ' << sigcodes << ' ' << keys::get_pubkey(pk);
	return os.str();
}


