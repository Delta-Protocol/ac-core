#ifndef USGOV_56c4ea14fe04c13aabcd1dd5ef4f05856a66f6515dcb1855f3046da43eea737a
#define USGOV_56c4ea14fe04c13aabcd1dd5ef4f05856a66f6515dcb1855f3046da43eea737a

#include <cstdint>
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "us/gov/socket/datagram.h"
#include "us/gov/id/daemon.h"
#include "us/gov/peer/daemon.h"
#include "peer_t.h"

namespace us { namespace gov { namespace relay {
using namespace std;
using socket::datagram;

class daemon: public peer::daemon {

private:

    typedef unordered_set<datagram::hash_t> evidences_t;
    evidences_t* m_evidences;


protected:

    virtual vector<peer_t*> get_nodes()=0;
    virtual bool process_evidence(datagram*)=0;
    virtual bool process_work(socket::peer_t *c, datagram*d) override;

public:

    daemon();
    daemon(uint16_t port, uint16_t edges);
    virtual ~daemon();

    void dump(ostream&os) const;

    using peer::daemon::send;
    void send(const datagram& g, socket::peer_t* exclude=0);

    evidences_t* retrieve_evidences(); //caller must take the lock
    mutable mutex m_mx_evidences;
};

}}}

#endif

