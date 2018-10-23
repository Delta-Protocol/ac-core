#include "app.h"
#include "engine.h"
#include <cassert>
#include "diff.h"
#include "auth_app.h"
#include "policies.h"
#include <us/gov/cash/app.h>
#include <us/gov/stacktrace.h>


using namespace us::gov::engine;

unsigned int app::get_seed() {
    if (get_chain_info().get_imported_last_block().empty()) 
        return 0;
    return *reinterpret_cast<const unsigned int*>(&get_chain_info().get_imported_last_block());  
}

uint64_t app::delta::merge(local_delta* other) {
    ++m_multiplicity;
    delete other;
    return 0;
}

app::local_delta* app::local_delta::create(int id) {
    if (id==auth::app::id()) 
        return new auth::app::local_delta();
    if (id==cash::app::id()) return new cash::app::local_delta();
        assert(false);
    return 0;
}

app::delta* app::delta::create(int id) {
    if (id==auth::app::id()) 
        return new auth::app::delta();
    if (id==cash::app::id()) 
        return new cash::app::delta();
    assert(false);
    return 0;
}


app::delta* app::delta::create(int id, istream& is) {
    if (id==auth::app::id()) 
        return auth::app::delta::from_stream(is);
    if (id==cash::app::id()) 
        return cash::app::delta::from_stream(is);
    return 0;
}

app::local_delta* app::local_delta::create(int id, istream& is) {
    local_delta* i = local_delta::create(id);
    if (!i) 
        return 0;
    i->from_stream(is);
    return i;
}

app::local_delta* app::local_delta::create(istream& is) {
    int id;
    is >> id;
    {
        string line;
        getline(is,line);
    }
    local_delta* i=local_delta::create(id);
    if (!i) 
        return 0;
    i->from_stream(is);
    return i;
}

app::delta* app::delta::create(istream& is) {
    int id;
    is >> id;
    return create(id,is);
}

string app::shell_command(const string&) {
    return "No shell available for this app.";
}
