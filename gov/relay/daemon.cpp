#include "daemon.h"
#include "peer_t.h"
#include "protocol.h"

typedef us::gov::relay::daemon c;
using namespace us::gov;
using namespace us::gov::relay;
using namespace std;

c::daemon() {
}
c::daemon(uint16_t port, uint16_t edges): b(port,edges) {
}

c::~daemon() {
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
        send(*d, c); //relay

		return process_evidence(d);
	}
//cout << "relay_dmn:passing downstream" << endl;
	return b::process_work(c,d);
}

void c::send(const datagram& g, socket::peer_t* exclude) {
    for (auto& i:get_nodes()) {
        if (i==exclude) continue; //dont relay to the original sender
        i->send(g);
    }
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

