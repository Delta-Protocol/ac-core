#include "peer_t.h"
#include "daemon.h"
#include "protocol.h"
#include "networking.h"
#include <us/gov/likely.h>

using namespace us::gov::engine;
using namespace std;

typedef us::gov::engine::peer_t c;

constexpr array<const char*,c::num_stages> c::stagestr;

void c::verification_completed() {
    b::verification_completed();

    if (unlikely(!verification_is_fine())) {
		cout << "disconnect peer, pubkey not verified." << endl;
		disconnect();
		return;
    }

    static_cast<networking*>(parent)->parent->auth_app->basic_auth_completed(this);

    if (unlikely(stage==sysop)) { // override logic of layer below
        if (static_cast<networking*>(parent)->parent->sysop_allowed) {
            us::gov::auth::peer_t::stage=us::gov::auth::peer_t::authorized;
            send(new datagram(us::gov::protocol::sysop,"go ahead"));
        }
        else {
            us::gov::auth::peer_t::stage=us::gov::auth::peer_t::denied;
        }
    }

    if (unlikely(us::gov::auth::peer_t::stage!=us::gov::auth::peer_t::authorized)) {
		cout << "disconnect peer, not authorized." << endl;
		disconnect();
		return;
    }
}


void c::dump(ostream& os) const {
	os << "engine: " << this << " stage: " << stagestr[stage] << endl;
}


const keys& c::get_keys() const {
    assert(parent != 0);
    return static_cast<id::daemon*>(parent)->get_keys();
}

bool c::authorize(const pubkey_t& p) const {
    if(unlikely(parent == 0)) return true;
    return not static_cast<id::daemon*>(parent)->is_duplicate(p);
}

