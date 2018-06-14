#ifndef USGOV_6d99739c50fa99bb398415cd2fdf9ab718b483010322b4471e8d174c2e9abb36
#define USGOV_6d99739c50fa99bb398415cd2fdf9ab718b483010322b4471e8d174c2e9abb36

#include <gov/auth.h>
#include <gov/blockchain.h>
#include "evidence.h"

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

	struct evidence_load: evidence {
        typedef evidence b;

		static evidence_load read(istream&);
		static evidence_load from_b58(const string&);
		virtual void write_sigmsg(ec::sigmsg_hasher_t&) const override;
		virtual void write(ostream&) const override;
		virtual void write_pretty(ostream& os) const override;

        datagram* get_datagram() const;

        bool load;
		hash_t item;

	};

}

static ostream& operator << (ostream&os, const nova::evidence_load& t) {
	os << t.to_b58() << endl;
	return os;
}


}

#endif

