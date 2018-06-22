#ifndef USGOV_273203cec00ddbe16562ae7fd257a2e5132867d8b90c56026878870da5b6ffee
#define USGOV_273203cec00ddbe16562ae7fd257a2e5132867d8b90c56026878870da5b6ffee

#include <us/gov/auth.h>
#include <us/gov/blockchain.h>
#include "evidence.h"

#include <vector>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <us/gov/signal_handler.h>
#include <us/gov/crypto/crypto.h>
#include "protocol.h"
#include <chrono>
#include <cassert>
#include <map>

namespace us{ namespace gov {
namespace nova {
	using namespace std;

	struct evidence_track: evidence {
        typedef evidence b;

		static evidence_track read(istream&);
		static evidence_track from_b58(const string&);

		virtual void write_sigmsg(ec::sigmsg_hasher_t&) const override;
		virtual void write(ostream&) const override;
		virtual void write_pretty(ostream& os) const override;

        datagram* get_datagram() const;

		string data;

	};

}

static ostream& operator << (ostream&os, const nova::evidence_track& t) {
	os << t.to_b58() << endl;
	return os;
}


}}

#endif

