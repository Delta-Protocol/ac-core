#include "daemon.h"
#include "protocol.h"

using namespace usgov;
using namespace std;
typedef usgov::wallet_daemon c;

c::wallet_daemon(uint16_t port, const string& home, const string&backend_host, uint16_t backend_port): b(port,2), wallet(home), backend_port(backend_port), backend_host(backend_host) {
}

c::~wallet_daemon() {
}

bool c::process_work(peer_t *c, datagram*d) {
	switch(d->service) {
		case protocol::wallet::balance_query: {
			refresh(backend_host, backend_port);
			ostringstream msg;
			msg << balance();
			c->send(protocol::wallet::balance_response,msg.str());
			this_thread::sleep_for(5s);
			delete d;
			return true;
		}
		break;
		default: break;
	}
	return false;
}

