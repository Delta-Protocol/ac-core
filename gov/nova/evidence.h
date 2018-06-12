#ifndef USGOV_7b76d8aadd7eb71bb498e2024c52966f72d21af3e24c96427a872e2f631a6daa
#define USGOV_7b76d8aadd7eb71bb498e2024c52966f72d21af3e24c96427a872e2f631a6daa

#include <gov/auth.h>
#include <gov/blockchain.h>

#include <vector>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <gov/signal_handler.h>
#include <gov/crypto/crypto.h>
//#include <blockchain/block.h>
#include "protocol.h"
#include <chrono>
#include <cassert>
#include <map>

namespace usgov {
namespace nova {
	using namespace std;
	using socket::datagram;
//	using blockchain::signature;
	using blockchain::local_deltas;
	using crypto::ripemd160;
	using crypto::ec;
	using blockchain::peer_t;

	typedef int64_t cash_t;
	typedef crypto::ec::keys keys;
	typedef keys::pub_t pubkey_t;

	typedef ripemd160 hasher_t;
	typedef hasher_t::value_type hash_t;

	struct evidence_load {
		ec::sigmsg_hasher_t::value_type get_hash() const;
		static evidence_load read(istream&);

		void write(ostream&) const;
		void write_sigmsg(ec::sigmsg_hasher_t&) const;
		void write_pretty(ostream& os) const;
		string to_b58() const;
		static evidence_load from_b58(const string&);

		datagram* get_datagram() const;

		hash_t item;
		hash_t compartiment;
		blockchain::diff::hash_t parent_block;

	};

	struct evidence_track {
		ec::sigmsg_hasher_t::value_type get_hash() const;
		static evidence_track read(istream&);

		void write(ostream&) const;
		void write_sigmsg(ec::sigmsg_hasher_t&) const;
		void write_pretty(ostream& os) const;
		string to_b58() const;
		static evidence_track from_b58(const string&);

		datagram* get_datagram() const;

		hash_t compartiment;
		string data;
		blockchain::diff::hash_t parent_block;

	};

}

static ostream& operator << (ostream&os, const nova::evidence_load& t) {
	os << t.to_b58() << endl;
	return os;
}
static ostream& operator << (ostream&os, const nova::evidence_track& t) {
	os << t.to_b58() << endl;
	return os;
}


}

#endif

