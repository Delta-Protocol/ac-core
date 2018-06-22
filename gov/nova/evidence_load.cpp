#include "evidence_load.h"
#include <us/gov/peer.h>
#include "protocol.h"
#include <thread>
#include <chrono>
#include <us/gov/crypto/base58.h>
#include <us/gov/likely.h>

typedef us::gov::nova::evidence_load c;
using namespace us::gov;
using namespace us::gov::nova;
using namespace std;

c c::read(istream& is) {
	c t;
	is >> t.item;
    is >> t.load;
    b::read(t,is);
	return move(t);
}

void c::write_sigmsg(ec::sigmsg_hasher_t& h) const {
	h.write(item);
	h.write(load);
    b::write_sigmsg(h);
}


void c::write_pretty(ostream& os) const {
	os << "---transaction---------------" << endl;
	os << "  item: " << item << endl;
    os << "  action: " << (load?"":"un") << "load" << endl;
    b::write_pretty(os);
	os << "-/-transaction---------------" << endl;
}

void c::write(ostream& os) const {
	os << item << ' ';
	os << load << ' ';
    b::write(os);
}


c c::from_b58(const string& s) {
	string txt=crypto::b58::decode(s);
//cout << txt << endl;
	istringstream is(txt);
	return read(is);
}

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


datagram* c::get_datagram() const {
	return new socket::datagram(protocol::nova_evidence_load,to_b58());
}


