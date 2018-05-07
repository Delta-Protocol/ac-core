#include "app.h"
#include "blockchain.h"
#include <cassert>
#include "diff.h"


using namespace usgov::blockchain;
typedef usgov::blockchain::app c;

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
#include <gov/cash/app.h>
#include <gov/rep/app.h>


local_delta* local_delta::create(int id) {
	if (id==auth::app::id()) return new auth::local_delta();
	if (id==cash::app::id()) return new cash::local_delta();
	if (id==rep::app::id()) return new rep::local_delta();
	return 0;
}

delta* delta::create(int id) {
	if (id==auth::app::id()) return new auth::delta();
	if (id==cash::app::id()) return new cash::delta();
	if (id==rep::app::id()) return new rep::delta();
	return 0;
}

delta* delta::create(int id, istream& is) {
	if (id==auth::app::id()) return auth::delta::from_stream(is);
	if (id==cash::app::id()) return cash::delta::from_stream(is);
	if (id==rep::app::id()) return rep::delta::from_stream(is);
	return 0;
}

local_delta* local_delta::create(int id, istream& is) {
	local_delta* i=local_delta::create(id);
	if (!i) return 0;
	i->from_stream(is);
	return i;	
}

local_delta* local_delta::create(istream& is) {
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

delta* delta::create(istream& is) {
	int id;
	is >> id;
	return create(id,is);
}

string c::shell_command(const string&) {
	return "No shell available for this app.";
}


