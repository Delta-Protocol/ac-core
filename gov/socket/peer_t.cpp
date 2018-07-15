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

