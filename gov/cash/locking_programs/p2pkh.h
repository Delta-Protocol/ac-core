#ifndef USGOV_a19083e9bc17d9b21b0e176de704d07cf9a659bc3bbe63f95d79e1696df47467
#define USGOV_a19083e9bc17d9b21b0e176de704d07cf9a659bc3bbe63f95d79e1696df47467

#include <gov/cash/app.h>

namespace usgov {
namespace cash {

	struct p2pkh {
		typedef crypto::ec::keys keys;
		typedef keys::pub_t pubkey_t;
		typedef keys::priv_t privkey_t;
		static hash_t locking_program_hash;

		static bool check_input(const hash_t& addreess, const tx&, const size_t& this_index, const string& locking_program_input);
		static string create_input(const tx& t, const size_t& this_index, const tx::sigcodes_t&, const privkey_t&);
		static string create_input(const ec::sigmsg_hasher_t::value_type&, const tx::sigcodes_t&, const privkey_t&);
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
}


#endif

