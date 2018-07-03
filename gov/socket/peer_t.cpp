#include "peer_t.h"
#include "daemon.h"
#include "protocol.h"

typedef us::gov::socket::peer_t c;
using namespace us::gov::socket;
using namespace std;
using namespace chrono_literals;

c::peer_t(): b(0) {
}

c::peer_t(int sock): b(sock) {
}

c::~peer_t() {
}

void c::ready() {
	parent->ready=true;
	parent->cv.notify_all();
}


void c::on_connect() {
	if (parent) parent->incorporate(this);
}

datagram* c::send_recv(const string&addr, uint16_t port, datagram*d) {
	peer_t cli;
	if (!cli.connect(addr,port,true)) {
		return 0;
	}
	cli.send(d);
	auto response=new datagram();
	while (!response->completed()) {
		if (!response->recv(cli.sock,2)) {
			delete response;
			return 0;
		}
	}
	return response;
}

