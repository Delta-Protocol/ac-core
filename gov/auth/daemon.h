#ifndef USGOV_c487b00055e53547e15c0855b22f83a7c8fb4700de16953d93bac9768cf373a9
#define USGOV_c487b00055e53547e15c0855b22f83a7c8fb4700de16953d93bac9768cf373a9

#include <gov/peer.h>
#include "peer_t.h"
#include <gov/socket.h>

namespace usgov {
namespace auth {
using namespace std;

struct daemon:peer::daemon {
	typedef peer::daemon b;
	typedef peer_t::keys keys;

	using datagram=socket::datagram;

	daemon(const keys&);
	daemon(const keys&, uint16_t port, uint16_t edges);
	virtual ~daemon();

	virtual socket::client* create_client(int sock) override;
	void dump(ostream& os) const;

    //virtual void on_connect(peer::peer_t&) override;

	virtual bool process_work(socket::peer_t *p, datagram*d) override;
	//virtual void report_in_service(vector<peer::peer_t*>&) override;

	keys id;
};


}
}

#endif

