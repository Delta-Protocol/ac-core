#include "daemon.h"
#include "protocol.h"
#include "peer_t.h"

using namespace us::gov;
using namespace us::gov::id;
using namespace std;


daemon::daemon(uint16_t port, uint16_t edges): socket::daemon(port,edges) {
}

socket::client* daemon::create_client(int sock) {
    return new peer_t(sock);
}

bool daemon::process_work(socket::peer_t *p, socket::datagram*d) {
    if (static_cast<peer_t*>(p)->process_work(d)) 
        return true;
    return socket::daemon::process_work(p,d);
}

void daemon::dump(ostream& os) const {
    os << "Hello from id::daemon" << endl;
    os << "This node public key: " << get_keys().get_pubkey() << endl;
    os << "active:" << endl;
    auto a=active();
    vector<peer_t*>& v=reinterpret_cast<vector<peer_t*>&>(a);
    for (auto& i:v) {
        i->dump(os);
    }
}

bool daemon::is_duplicate(const pub_t& p) const {
    vector<socket::client*> a=active();
    int count = 0;
    for (auto&i:a) {
        if (p==static_cast<peer_t*>(i)->get_pubkey()) {
            count++;
            if(count > 1)
                return true;
        }
    }
    return false;
}



