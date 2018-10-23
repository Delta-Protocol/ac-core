#include "peer_t.h"
#include "daemon.h"
#include "protocol.h"
#include "networking.h"
#include <us/gov/likely.h>

using namespace us::gov::engine;
using namespace std;

constexpr array<const char*,peer_t::num_stages> peer_t::m_stagestr;

void peer_t::verification_completed() {
    b::verification_completed();

    if (unlikely(!verification_is_fine())) {
        cout << "disconnect peer, pubkey not verified." << endl;
        disconnect();
        return;
    }


    static_cast<networking*>(m_parent)->get_parent()->get_auth_app()->basic_auth_completed(this);

    if (unlikely(m_stage==sysop)) {
        if (static_cast<networking*>(m_parent)->get_parent()->sysop_allowed) {
            us::gov::auth::peer_t::set_stage(us::gov::auth::peer_t::authorized);
            send(new datagram(us::gov::protocol::sysop,"go ahead"));
        }else {
            us::gov::auth::peer_t::set_stage(us::gov::auth::peer_t::denied);
        }
    }

    if (unlikely(us::gov::auth::peer_t::get_stage()!=us::gov::auth::peer_t::authorized)) {
        cout << "disconnect peer, not authorized." << endl;
        disconnect();
        return;
    }
}

void peer_t::dump(ostream& os) const {
    os << "engine: " << this << " stage: " << stagestr[m_stage] << endl;
}

const keys& peer_t::get_keys() const {
    assert(m_parent != 0);
    return static_cast<id::daemon*>(m_parent)->get_keys();
}

bool peer_t::authorize(const pubkey_t& p) const {
    if(unlikely(m_parent == 0))
        return true;
    return not static_cast<id::daemon*>(m_parent)->is_duplicate(p);
}

