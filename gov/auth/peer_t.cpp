#include "peer_t.h"

typedef us::gov::auth::peer_t c;
using namespace std;

constexpr array<const char*,c::num_stages> c::stagestr;

c::peer_t(int sock):b(sock) {
}

c::~peer_t() {
}
 
void c::verification_completed() {
    b::verification_completed();
	if (authorize(pubkey)) {
		stage=authorized;
	}
}

bool c::authorize(const pubkey_t& p) const {
	return true;
}

void c::dump(ostream& os) const {
    os << this << ' ' << pubkey << "- " << stagestr[stage] << endl;
}

