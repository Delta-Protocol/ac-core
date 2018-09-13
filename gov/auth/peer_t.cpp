#include "peer_t.h"

typedef us::gov::auth::peer_t c;
using namespace std;

constexpr array<const char*,c::num_stages> c::stagestr;

c::peer_t(int sock):b(sock) {
}

c::~peer_t() {
}

#include <us/gov/engine/daemon.h>

void c::verification_completed() {
    //b::verification_completed();
    if (unlikely(!verification_is_fine())) {
        return;
    }
   	if (unlikely(!authorize(pubkey))) {
        return;
   	}
   	stage=authorized;
}
void c::dump(ostream& os) const {
    os << "auth: " << this << ' ' << pubkey << " " << stagestr[stage] << endl;
}

