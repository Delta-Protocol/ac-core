#include "daemon.h"

#include <stddef.h>
#include <utility>

#include "us/gov/peer/peer_t.h"
#include "protocol.h"

using namespace us::gov;
using namespace us::gov::relay;
using namespace std;

daemon::daemon() {
    evidences=new evidences_t();
}
daemon::daemon(uint16_t port, uint16_t edges): peer::daemon(port,edges) {
    evidences=new evidences_t();
}

daemon::~daemon() {
    delete evidences;
}

daemon::evidences_t* daemon::retrieve_evidences() { //caller must take the lock
    auto e=evidences;
    evidences=new evidences_t();
    return e;
}

bool daemon::process_work(socket::peer_t *c, datagram*d) {
    if (protocol::is_evidence(d->service)) {
        datagram::hash_t h=d->compute_hash();
        {
            unique_lock<mutex> lock(mx_evidences);
            if (evidences->find(h)!=evidences->end()) {
                lock.unlock();
                delete d;
                return true;
            }
            evidences->emplace(h);
        }
        send(*d, c); //relay
        return process_evidence(d);
    }

    return peer::daemon::process_work(c,d);
}

void daemon::send(const datagram& g, socket::peer_t* exclude) {
    for (auto& i:get_nodes()) {
        if (i==exclude) continue; //dont relay to the original sender
        i->send(g);
    }
}

void daemon::dump(ostream& os) const {
    os << "Hello from relay::daemon" << endl;
    size_t z;
    {
        lock_guard<mutex> lock(mx_evidences);
        z=evidences->size();
    }

    os << "Unique evidences this cycle: " << z << endl;
}
