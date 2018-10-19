/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#include "daemon.h"

#include <stddef.h>
#include <utility>

#include "us/gov/peer/peer_t.h"
#include "protocol.h"

using namespace us::gov;
using namespace us::gov::relay;
using namespace std;

daemon::daemon() {
    m_evidences=new evidences_t();
}
daemon::daemon(uint16_t port, uint16_t edges): peer::daemon(port,edges) {
    m_evidences=new evidences_t();
}

daemon::~daemon() {
    delete m_evidences;
}

daemon::evidences_t* daemon::retrieve_evidences() { //caller must take the lock
    auto e=m_evidences;
    m_evidences=new evidences_t();
    return e;
}

bool daemon::process_work(socket::peer_t *c, datagram*d) {
    if (protocol::is_evidence(d->service)) {
        datagram::hash_t h=d->compute_hash();
        {
            unique_lock<mutex> lock(m_mx_evidences);
            if (m_evidences->find(h)!=m_evidences->end()) {
                lock.unlock();
                delete d;
                return true;
            }
            m_evidences->emplace(h);
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
        lock_guard<mutex> lock(m_mx_evidences);
        z=m_evidences->size();
    }

    os << "Unique evidences this cycle: " << z << endl;
}
