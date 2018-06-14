#include "evidence_track.h"
#include <us/gov/peer.h>
#include "protocol.h"
#include <thread>
#include <chrono>
#include <us/gov/crypto/base58.h>
#include <us/gov/likely.h>

typedef us::gov::nova::evidence_track c;
using namespace us::gov;
using namespace us::gov::nova;
using namespace std;

c c::read(istream& is) {
	c t;
	is >> t.data;
    t.data=crypto::b58::decode(t.data);
    b::read(t,is);
	return move(t);
}

void c::write_sigmsg(ec::sigmsg_hasher_t& h) const {
	h.write(data);
    b::write_sigmsg(h);
}

void c::write_pretty(ostream& os) const {
	os << "---transaction---------------" << endl;
	os << "  data: " << data << endl;
    b::write_pretty(os);
	os << "-/-transaction---------------" << endl;
}

void c::write(ostream& os) const {
	os << crypto::b58::encode(data) << ' ';
    b::write(os);
}

c c::from_b58(const string& s) {
	string txt=crypto::b58::decode(s);
	istringstream is(txt);
	return read(is);
}

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

datagram* c::get_datagram() const {
	return new socket::datagram(protocol::nova_evidence_track,to_b58());
}


