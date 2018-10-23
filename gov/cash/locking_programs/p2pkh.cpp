#include "p2pkh.h"
#include <sstream>
#include <us/gov/crypto/crypto.h>

using namespace us::gov;
using namespace us::gov::cash;

hash_t p2pkh::locking_program_hash{1};

bool p2pkh::check_input(const hash_t& addreess, 
                        const tx& t, const size_t& this_index, 
                        const std::string& locking_program_input) {
    
    std::istringstream is(locking_program_input);
    keys::pub_t pk;
    std::string signature_der_b58;
    tx::sigcodes_t sigcodes;
    is >> signature_der_b58;
    is >> sigcodes;
    is >> pk;
    if (pk.hash()!=addreess) {
        std::cout << "Address check failed." << std::endl;
        return false;
    }

    if (!ec::get_instance().verify(pk, t.get_hash(this_index, sigcodes),signature_der_b58)) {
        return false;
    }
    return true;
}

string p2pkh::create_input(const tx& t, const size_t& this_index, 
                           const tx::sigcodes_t& sigcodes, 
                           const privkey_t& pk)                                {
    return create_input(t.get_hash(this_index, sigcodes),sigcodes,pk);
}

string p2pkh::create_input(const ec::sigmsg_hasher_t::value_type& h,
                           const tx::sigcodes_t& sigcodes, const privkey_t& pk) {
    std::ostringstream os;
    os << ec::get_instance().sign_encode(pk,h) << ' ' << sigcodes 
                                         << ' ' << keys::get_pubkey(pk);
    return os.str();
}


