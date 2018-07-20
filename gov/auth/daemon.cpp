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

bool c::process_work(socket::peer_t *p, datagram*d) {
    if (b::process_work(p,d)) return true;
cout << "auth dmn: I should process tyhis" << endl;
    return static_cast<peer_t*>(p)->process_work(d);
}

void c::dump(ostream& os) const {
	os << "Hello from auth::daemon" << endl;
    os << "active:" << endl;
    auto a=active();
    vector<peer_t*>& v=reinterpret_cast<vector<peer_t*>&>(a);
    for (auto& i:v) {
        i->dump(os);
    }
}


