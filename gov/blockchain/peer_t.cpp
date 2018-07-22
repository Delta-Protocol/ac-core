#include "peer_t.h"
#include "daemon.h"
#include "protocol.h"
#include <us/gov/likely.h>

using namespace us::gov::blockchain;
using namespace std;

typedef us::gov::blockchain::peer_t c;

constexpr array<const char*,c::num_stages> c::stagestr;

void c::verification_completed() {
    b::verification_completed();

	cout << "blockchain: daemon: verification_completed" << endl;
    if (unlikely(us::gov::auth::peer_t::stage!=us::gov::auth::peer_t::authorized)) {
		cout << "disconnected peer, not authorized." << endl;
		disconnect();
		return;
    }
	static_cast<daemon::networking*>(parent)->parent->auth_app->basic_auth_completed(this);

    if (unlikely(stage==sysop)) {
        if (!static_cast<daemon::networking*>(parent)->parent->sysop_allowed) {
            disconnect();
        }
        else {
            send(new datagram(us::gov::protocol::sysop,"go ahead"));
        }
    }
}


void c::dump(ostream& os) const {
	os << this << " stage: " << stagestr[stage] << endl;
}


const keys& c::get_keys() const {
    return static_cast<daemon::networking*>(parent)->get_keys();
}

bool c::authorize(const pubkey_t&) const { //all identified nodes are authorized
    return true;
}

