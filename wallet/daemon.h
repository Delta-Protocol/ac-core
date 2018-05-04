#ifndef USGOV_98e8c16336dfe2bb0f4b26ffe37d808f974893b94c081f2f0f0419f11ac5e0ae
#define USGOV_98e8c16336dfe2bb0f4b26ffe37d808f974893b94c081f2f0f0419f11ac5e0ae

#include <gov/socket/daemon.h>
#include "wallet.h"

namespace usgov {

using namespace std;

using socket::datagram;

struct wallet_daemon: socket::daemon, wallet {
	typedef socket::daemon b;
	typedef socket::peer_t peer_t;

	wallet_daemon(uint16_t port, const string& home, const string&backend_host, uint16_t backend_port);
	virtual ~wallet_daemon();
	bool process_work(peer_t*, datagram*);

    string backend_host;
    uint16_t backend_port;

};


}

#endif

