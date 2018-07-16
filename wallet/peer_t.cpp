#include "peer_t.h"

typedef us::wallet::peer_t c;
using namespace std;

constexpr array<const char*,c::num_stages> c::stagestr;

c::peer_t(const keys& k, int sock):b(sock),k(k) {
}

c::~peer_t() {
}
 
const c::keys& c::get_keys() const {
    return k;
}

void c::verification_completed() {

    pair<string,datagram*> r=recv();

    if (r.first.empty()) {
        if (r.second->parse_string()=="authorized") {
            stage=authorized;
        }
        delete r.second;
    }
    assert(r.second==0);
}

