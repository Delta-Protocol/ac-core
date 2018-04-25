#include "peer_t.h"
#include "daemon.h"

using namespace usgov::blockchain;
using namespace std;

typedef usgov::blockchain::peer_t c;

constexpr array<const char*,c::num_stages> c::stagestr;

void c::verification_completed() {
	cout << "blockchain: daemon: verification_completed" << endl;
	if (stage_peer!=usgov::auth::peer_t::verified || stage_me!=usgov::auth::peer_t::verified) {
		cout << "disconnected peer, both sides successful verification is required." << endl;
		disconnect();
		return;
	}
	reinterpret_cast<daemon::networking*>(parent)->parent->auth_app->basic_auth_completed(this);
}


void c::dump(ostream& os) const {
	//b::dump(os);
	os << this << " stage: " << stagestr[stage] << endl;
}


