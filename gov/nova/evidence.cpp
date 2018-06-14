#include "evidence.h"
#include <us/gov/peer.h>
#include "protocol.h"
#include <thread>
#include <chrono>
#include <us/gov/crypto/base58.h>
#include <us/gov/likely.h>

typedef us::gov::nova::evidence c;
using namespace us::gov;
using namespace us::gov::nova;
using namespace std;
		static void read(evidence&, istream&);

void c::read(c& t, istream& is) {
	is >> t.compartiment;
	string locking_program_input_b58;
	is >> locking_program_input_b58;
	t.locking_program_input=crypto::b58::decode(locking_program_input_b58);
	if (t.locking_program_input=="-") t.locking_program_input.clear();
	is >> t.parent_block;
}

void c::write_sigmsg(ec::sigmsg_hasher_t& h) const {
	h.write(compartiment);
	h.write(locking_program_input);
	h.write(parent_block);
}


void c::write_pretty(ostream& os) const {
	os << "  compartiment: " << compartiment << endl;
	os << "  parent_block: " << parent_block << endl;
}

void c::write(ostream& os) const {
	os << compartiment << ' ';
	os << crypto::b58::encode(locking_program_input.empty()?"-":locking_program_input) << ' ';
	os << parent_block << ' ';
}

string c::to_b58() const {
	ostringstream os;
	write(os);
	return crypto::b58::encode(os.str());
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



