#ifndef USGOV_56c4ea14fe04c13aabcd1dd5ef4f05856a66f6515dcb1855f3046da43eea737a
#define USGOV_56c4ea14fe04c13aabcd1dd5ef4f05856a66f6515dcb1855f3046da43eea737a

//#include <us/gov/auth/daemon.h>
#include <us/gov/peer/daemon.h>
#include "peer_t.h"
#include <unordered_set>
#include <iostream>
#include <mutex>

namespace us{ namespace gov {
namespace relay {
using namespace std;

	using socket::datagram;
//	struct daemon : auth::daemon {
	struct daemon:peer::daemon {
//		typedef auth::daemon b;
		typedef peer::daemon b;
		daemon();
 		daemon(uint16_t port, uint16_t edges);
		virtual ~daemon();
//        virtual socket::client* create_client(int sock) override;
		void clear_evidences();
		void dump(ostream&os) const;

		virtual bool process_work(socket::peer_t *c, datagram*d) override;
		virtual bool process_evidence(peer_t *c, datagram*d);

		typedef unordered_set<datagram::hash_t> evidences_t;
		evidences_t evidences;
		mutable mutex mx_evidences;

	};

}

}}

#endif

