#include "daemon.h"
#include "protocol.h"
#include "peer_t.h"

using namespace us::gov;
using namespace us::gov::id;
using namespace std;

typedef us::gov::id::daemon c;


c::daemon(uint16_t port, uint16_t edges): b(port,edges) {
}

c::daemon() {
}

c::~daemon() {
}

socket::client* c::create_client(int sock) {
    return new peer_t(sock);
//    p->parent=this;
//    return p;
}

bool c::process_work(socket::peer_t *p, datagram*d) {
    if (static_cast<peer_t*>(p)->process_work(d)) return true;
    return b::process_work(p,d);
//    return static_cast<peer_t*>(p)->process_work(d);
}

void c::dump(ostream& os) const {
	os << "Hello from id::daemon" << endl;
	os << "This node public key: " << get_keys().pub << endl;
	os << "active:" << endl;
	auto a=active();
	vector<peer_t*>& v=reinterpret_cast<vector<peer_t*>&>(a);
	for (auto& i:v) {
		i->dump(os);
	}
}

bool c::is_duplicate(const pub_t& p) const {
    vector<socket::client*> a=active();
    int count = 0;
    for (auto&i:a) {
        if (p==static_cast<peer_t*>(i)->pubkey) {
            count++;
            if(count > 1)
                return true;
        }
    }
    return false;
}







