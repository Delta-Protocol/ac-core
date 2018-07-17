#include "daemon.h"
#include "peer_t.h"

using namespace us::gov;
using namespace us::gov::auth;
using namespace std;

typedef us::gov::auth::daemon c;


c::daemon(uint16_t port, uint16_t edges): b(port,edges) {
}

c::daemon() {
}

c::~daemon() {
}
/*
socket::client* c::create_client(int) {
	cerr << "this function have to ve overriden" << endl;
	exit(1);
}
*/
bool c::process_work(socket::peer_t *p, datagram*d) {
    if (b::process_work(p,d)) return true;
    return static_cast<peer_t*>(p)->process_work(d);
}

void c::dump(ostream& os) const {
	os << "Hello from auth::daemon" << endl;
}


