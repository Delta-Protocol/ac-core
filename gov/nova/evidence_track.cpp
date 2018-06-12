#include "evidence.h"
#include <gov/peer.h>
#include "protocol.h"
#include <thread>
#include <chrono>
#include <gov/crypto/base58.h>
#include <gov/likely.h>

typedef usgov::nova::evidence_track c;
using namespace usgov;
using namespace usgov::nova;
using namespace std;

c c::read(istream& is) {
	c t;
	is >> t.compartiment;
	is >> t.data;
	is >> t.parent_block;
	return move(t);
}

void c::write_sigmsg(ec::sigmsg_hasher_t& h) const {
	h.write(compartiment);
	h.write(data);
	h.write(parent_block);
}

void c::write_pretty(ostream& os) const {
	os << "---transaction---------------" << endl;
	os << "  parent_block: " << parent_block << endl;
	os << "  compartiment: " << compartiment << endl;
	os << "  data: " << data << endl;
	os << "-/-transaction---------------" << endl;
}

void c::write(ostream& os) const {
	os << compartiment << ' ';
	os << data << ' ';
	os << parent_block << ' ';
}

string c::to_b58() const {
	ostringstream os;
	write(os);
	return crypto::b58::encode(os.str());
}

c c::from_b58(const string& s) {
	string txt=crypto::b58::decode(s);
	istringstream is(txt);
	return read(is);
}

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

ec::sigmsg_hasher_t::value_type c::get_hash() const {
	ec::sigmsg_hasher_t h;
	write_sigmsg(h);
	ec::sigmsg_hasher_t::value_type v;
	h.finalize(v);
//cout << "sighash for index " << this_index << " sigcodes " << (int)sc << " " << v << endl;
	return move(v);
}

datagram* c::get_datagram() const {
	return new socket::datagram(protocol::nova_evidence_track,to_b58());
}


