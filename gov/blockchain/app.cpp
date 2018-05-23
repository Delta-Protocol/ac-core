#include "app.h"
#include "blockchain.h"
#include <cassert>
#include "diff.h"


using namespace us::gov::blockchain;
typedef us::gov::blockchain::app c;

//unsigned int c::rng_seed{0};
diff::hash_t c::last_block_imported{0};

unsigned int c::get_seed() {
	if (last_block_imported.empty()) return 0;
	return *reinterpret_cast<const unsigned int*>(&last_block_imported);
}

/*
*/
#include "auth_app.h"
#include "policies.h"
#include <us/gov/cash/app.h>
#include <us/gov/rep/app.h>


uint64_t c::delta::merge(local_delta* other) {
	++multiplicity;
	delete other;
	return 0;
}

c::local_delta* c::local_delta::create(int id) {
	if (id==auth::app::id()) return new auth::app::local_delta();
	if (id==cash::app::id()) return new cash::app::local_delta();
	if (id==rep::app::id()) return new rep::local_delta();
	return 0;
}

c::delta* c::delta::create(int id) {
	if (id==auth::app::id()) return new auth::app::delta();
	if (id==cash::app::id()) return new cash::app::delta();
	if (id==rep::app::id()) return new rep::delta();
	return 0;
}

c::delta* c::delta::create(int id, istream& is) {
	if (id==auth::app::id()) return auth::app::delta::from_stream(is);
	if (id==cash::app::id()) return cash::app::delta::from_stream(is);
	if (id==rep::app::id()) return rep::delta::from_stream(is);
	return 0;
}

c::local_delta* c::local_delta::create(int id, istream& is) {
	local_delta* i=local_delta::create(id);
	if (!i) return 0;
	i->from_stream(is);
	return i;	
}

c::local_delta* c::local_delta::create(istream& is) {
	int id;
	is >> id;
	{
	string line;
	getline(is,line);
	}
	local_delta* i=local_delta::create(id);
	if (!i) return 0;
	i->from_stream(is);
	return i;
}

c::delta* c::delta::create(istream& is) {
	int id;
	is >> id;
	return create(id,is);
}

string c::shell_command(const string&) {
	return "No shell available for this app.";
}


