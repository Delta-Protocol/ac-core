#ifndef US_GOV_CASH_LOCKING_PROGRAMS_P2PKH_H
#define US_GOV_CASH_LOCKING_PROGRAMS_P2PKH_H

#include <us/gov/cash/app.h>
#include <us/gov/cash/tx.h>

namespace us{ namespace gov{ namespace cash{

class p2pkh {
public:
    typedef crypto::ec::keys keys;
    typedef keys::pub_t pubkey_t;
    typedef keys::priv_t privkey_t;
        
    static  hash_t locking_program_hash;

    static bool check_input(const hash_t& addreess, 
                            const tx&, const size_t& this_index, 
                            const string& locking_program_input);

    static string create_input(const tx& t, const size_t& this_index,
                               const tx::sigcodes_t&, const privkey_t&);

    static string create_input(const ec::sigmsg_hasher_t::value_type&,
                               const tx::sigcodes_t&, const privkey_t&);
};
}}}


#endif

