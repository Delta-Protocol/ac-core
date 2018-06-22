#ifndef USGOV_7b76d8aadd7eb71bb498e2024c52966f72d21af3e24c96427a872e2f631a6dab
#define USGOV_7b76d8aadd7eb71bb498e2024c52966f72d21af3e24c96427a872e2f631a6dab

#include <us/gov/auth.h>
#include <us/gov/blockchain.h>
#include <us/gov/socket.h>

#include <vector>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <us/gov/signal_handler.h>
#include <us/gov/crypto/crypto.h>
//#include <blockchain/block.h>
#include "protocol.h"
#include <chrono>
#include <cassert>
#include <map>

namespace us{ namespace gov {
namespace nova {
	using namespace std;
	using namespace us::gov;
	using socket::datagram;
	using blockchain::local_deltas;
	using crypto::ripemd160;
	using crypto::ec;
	using blockchain::peer_t;

	typedef int64_t cash_t;
	typedef us::gov::crypto::ec::keys keys;
	typedef keys::pub_t pubkey_t;

	typedef ripemd160 hasher_t;
	typedef hasher_t::value_type hash_t;

	struct evidence {

		static void read(evidence&, istream&);

		ec::sigmsg_hasher_t::value_type get_hash() const;

		virtual void write(ostream&) const;
		virtual void write_sigmsg(ec::sigmsg_hasher_t&) const;
		virtual void write_pretty(ostream& os) const;
		string to_b58() const;

		hash_t compartiment;
		hash_t locking_program;

        string locking_program_input;
		blockchain::diff::hash_t parent_block;
    };



}


}}

#endif

