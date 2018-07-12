#include "daemon.h"
#include "protocol.h"
#include <us/gov/auth.h>

using namespace us::gov;
using namespace us::gov::auth;
using namespace std;

typedef us::gov::auth::daemon c;


c::daemon(const keys& k, uint16_t port, uint16_t edges): b(port,edges), id(k) {
}

c::daemon(const keys& k): id(k) {
}

c::~daemon() {
}

socket::client* c::create_client(int sock) {
	auto p=new peer_t(sock);
	p->parent=this;
	return p;
}

bool c::process_work(socket::peer_t *p, datagram*d) {
	if (b::process_work(p,d)) return true;
	peer_t *peer=static_cast<peer_t*>(p);

    return peer->process_work(d);
}

void c::dump(ostream& os) const {
	os << "Hello from auth::daemon" << endl;
	os << "This node public key: " << id.pub << endl;
	os << "active:" << endl;
	auto a=active();
	vector<peer_t*>& v=reinterpret_cast<vector<peer_t*>&>(a);
	for (auto& i:v) {
		i->dump(os);
	}
}







