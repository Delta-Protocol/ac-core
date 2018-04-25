#include "app.h"
#include "blockchain.h"
#include <cassert>
#include "diff.h"


using namespace usgov::blockchain;
typedef usgov::blockchain::app c;

const c::keys& c::get_keys() const {
	assert(parent!=0);
	return parent->peerd.id; 
}

#include "auth_app.h"
#include "policies.h"
#include <gov/cash/app.h>
#include <gov/rep/app.h>

app_gut* app_gut::create(int id) {
	if (id==auth::app::id()) return new auth::app_gut();
	if (id==cash::app::id()) return new cash::app_gut();
	if (id==rep::app::id()) return new rep::app_gut();
	return 0;
}

app_gut2* app_gut2::create(int id) {
	if (id==auth::app::id()) return new auth::app_gut2();
	if (id==cash::app::id()) return new cash::app_gut2();
	if (id==rep::app::id()) return new rep::app_gut2();
	return 0;
}

app_gut2* app_gut2::create(int id, istream& is) {
	if (id==auth::app::id()) return auth::app_gut2::from_stream(is);
	if (id==cash::app::id()) return cash::app_gut2::from_stream(is);
	if (id==rep::app::id()) return rep::app_gut2::from_stream(is);
	return 0;
}

app_gut* app_gut::create(int id, istream& is) {
	app_gut* i=app_gut::create(id);
	if (!i) return 0;
	i->from_stream(is);
	return i;	
}

app_gut* app_gut::create(istream& is) {
	int id;
	is >> id;
	{
	string line;
	getline(is,line);
	}
	app_gut* i=app_gut::create(id);
	if (!i) return 0;
	i->from_stream(is);
	return i;
}

app_gut2* app_gut2::create(istream& is) {
	int id;
	is >> id;
	return create(id,is);
}

