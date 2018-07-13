#include "peer_t.h"
#include "daemon.h"
#include "protocol.h"
#include <us/gov/likely.h>

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

pair<string,datagram*> c::send_recv(datagram* d) {
    pair<string,datagram*> ans;
	ans.first=send(d);
    if (unlikely(!ans.first.empty())) {
        return move(ans);
    }
    return recv_response();
}
/*
pair<string,datagram*> c::send_recv(const string&addr, uint16_t port, datagram*d) {
	peer_t cli;
	if (!cli.connect(addr,port,true)) {
		return 0;
	}

    recv_response(socket::response_timeout_secs);

	auto response=new datagram();
	while (!response->completed()) {
		auto e=response->recv(cli.sock,socket::timeout_);
		if (!e.empty()) {
			delete response;
			return make_pair(e,0);
		}
	}
	return make_pair("",;
}
*/
