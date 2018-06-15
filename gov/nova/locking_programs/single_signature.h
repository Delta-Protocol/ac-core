#ifndef USGOV_2842d9be08f4c6ffe9bcb224695c3a52eda5808d21b6160fb9c3660681963bd0
#define USGOV_2842d9be08f4c6ffe9bcb224695c3a52eda5808d21b6160fb9c3660681963bd0

#include <us/gov/nova/app.h>

namespace us{ namespace gov {
namespace nova {

	struct single_signature {
		typedef crypto::ec::keys keys;
		typedef keys::pub_t pubkey_t;
		typedef keys::priv_t privkey_t;
		static hash_t locking_program_hash;

		static bool check_input(const hash_t& compartiment, const evidence&, const string& locking_program_input);
		static string create_input(const evidence& t, const privkey_t&);
		static string create_input(const ec::sigmsg_hasher_t::value_type&, const privkey_t&);
	};
/*
	struct multisig {
		typedef app::accounts_t::tx tx;
		typedef crypto::ec::keys keys;
		typedef keys::pub_t pubkey_t;
		typedef keys::priv_t privkey_t;
		static hash_t locking_program_hash;

		static bool check_input(const hash_t& addreess, const tx&, const size_t& this_index, const string& locking_program_input);
		static string create_input(const tx& t, const size_t& this_index, const tx::sigcodes_t&, const privkey_t&);
		static string create_input(const ec::sigmsg_hasher_t::value_type&, const tx::sigcodes_t&, const privkey_t&);
	};
*/

}
}}


#endif

