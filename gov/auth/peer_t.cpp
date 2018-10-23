#include "peer_t.h"
#include <us/gov/engine/daemon.h>

using namespace us::gov::auth;
using namespace std;

constexpr array<const char*,peer_t::num_stages> peer_t::m_stagestr;

void peer_t::verification_completed() {
    if (unlikely(!verification_is_fine()) || unlikely(!authorize(get_pubkey()))) {
        return;
    }
    m_stage=authorized;
}
void peer_t::dump(ostream& os) const {
    os << "auth: " << this << ' ' << get_pubkey() << " " << m_stagestr[m_stage] << endl;
}

