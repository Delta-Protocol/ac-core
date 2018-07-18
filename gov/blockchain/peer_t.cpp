#include "peer_t.h"
#include "daemon.h"

using namespace us::gov::blockchain;
using namespace std;

typedef us::gov::blockchain::peer_t c;

constexpr array<const char*,c::num_stages> c::stagestr;

void c::verification_completed() {
    b::verification_completed();

	cout << "blockchain: daemon: verification_completed" << endl;
    if (us::gov::auth::peer_t::stage!=us::gov::auth::peer_t::authorized) {
		cout << "disconnected peer, not authorized." << endl;
		disconnect();
		return;
    }
/*
	if (stage_peer!=us::gov::auth::id_peer::verified || stage_me!=us::gov::auth::id_peer::verified) {
		cout << "disconnected peer, both sides successful verification is required." << endl;
		disconnect();
		return;
	}
*/
	static_cast<daemon::networking*>(parent)->parent->auth_app->basic_auth_completed(this);
}


void c::dump(ostream& os) const {
	//b::dump(os);
	os << this << " stage: " << stagestr[stage] << endl;
}


const keys& c::get_keys() const {
    return static_cast<daemon::networking*>(parent)->get_keys();
}

