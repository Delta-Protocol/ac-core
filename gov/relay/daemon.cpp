#include "daemon.h"
#include "peer_t.h"
#include "protocol.h"

typedef us::gov::relay::daemon c;
using namespace us::gov;
using namespace us::gov::relay;
using namespace std;

c::daemon(const keys& k): b(k) {
}
c::daemon(const keys& k, uint16_t port, uint16_t edges): b(k, port,edges) {
}

c::~daemon() {
}

socket::client* c::create_client(int sock) {
	auto p=new peer_t(sock);
	p->parent=this;
	return p;
}

bool c::process_evidence(peer_t *c, datagram*d) {
	return false;
}

void c::clear_evidences() {
	evidences.clear();
}

bool c::process_work(socket::peer_t *c, datagram*d) {
	if (protocol::is_evidence(d->service)) {
		datagram::hash_t h=d->compute_hash();
		{
		lock_guard<mutex> lock(mx_evidences);
		if (evidences.find(h)!=evidences.end()) {
		delete d;
		return true;
		}
		evidences.emplace(h);
		}
		return process_evidence(static_cast<peer_t*>(c),d);
	}
	return b::process_work(c,d);
}



void c::dump(ostream& os) const {
	os << "Hello from relay::daemon" << endl;
	size_t z;
        {
	lock_guard<mutex> lock(mx_evidences);
	z=evidences.size();
	}

	os << "Unique evidences this cycle: " << z << endl;

}

