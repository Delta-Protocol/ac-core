#include "daemon.h"
#include "protocol.h"
#include <gov/auth.h>

using namespace usgov;
using namespace usgov::auth;
using namespace std;

typedef usgov::auth::daemon c;


c::daemon(const keys& k): id(k) {
}

c::daemon(const keys& k, uint16_t port, uint16_t edges): b(port,edges), id(k) {
}

c::~daemon() {
}

/*
void c::on_connect(peer::peer_t& p) {
    b::on_connect(p);
    cout << "ON CONNECT" << endl;
    static_cast<peer_t&>(p).do_actions();
}
*/

socket::client* c::create_client(int sock) {
	auto p=new peer_t(sock);
	p->parent=this;
	return p;
}

bool c::process_work(socket::peer_t *p, datagram*d) {
//cout << "AUTH: process work0" << endl;
	if (b::process_work(p,d)) return true;
//cout << "AUTH: process work" << endl;
	peer_t *peer=static_cast<peer_t*>(p);

	assert(d!=0);
	switch(d->service) {
		case protocol::auth_request: peer->process_auth_request(d,id); break;
		case protocol::auth_peer_challenge: peer->process_auth_peer_challenge(d,id); break;
		case protocol::auth_challenge_response: peer->process_auth_challenge_response(d); break;
		case protocol::auth_peer_status: peer->process_auth_peer_status(d); break;
	default: return false;
	}
	return true;
}
/*
void c::report_in_service(vector<peer::peer_t*>& isp) {
}
*/
void c::dump(ostream& os) const {
	os << "Hello from auth::daemon" << endl;
	os << "This node public key: " << id.pub << endl;
	os << "active:" << endl;
	auto a=active();
	vector<peer_t*>& v=reinterpret_cast<vector<peer_t*>&>(a);
	for (auto& i:v) {
		i->dump(os);
	}
}







