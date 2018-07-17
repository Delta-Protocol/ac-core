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
/*
    pair<string,datagram*> r=recv();

    if (r.first.empty()) {
        if (r.second->parse_string()=="authorized") {
            stage=authorized;
        }
        delete r.second;
    }
    assert(r.second==0);
*/
}

string c::run_auth() {
	auto r=b::run_auth();
	if (!r.empty()) return r;

	return "";
}

