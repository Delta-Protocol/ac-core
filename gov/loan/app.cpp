#include "app.h"

#include <mutex>
#include <sstream>

using namespace us::gov;
using namespace us::gov::loan;
using namespace std;

constexpr const char* app::name;
constexpr array<const char*,policies_traits::num_params> policies_traits::paramstr;

int local_delta::app_id() const {
    return app::id();
}

app::app() {
    pool=new loan::local_delta();
    policies_local=policies;
}

app::~app() {
    delete pool;
}

void app::add_policies() {
    lock_guard<mutex> lock(mx_pool);
    lock_guard<mutex> lock2(mx_policies);
    for (int i=0; i<policies_traits::num_params; ++i)
        (*pool)[i] = policies_local[i];
}

engine::app::local_delta* app::create_local_delta() {
    add_policies();
    lock_guard<mutex> lock(mx_pool);
    auto full=pool;
    pool=new loan::local_delta();
    return full; //send collected transactions to the network
}

void app::dbhash(hasher_t&) const {
}

void app::db_t::dump(ostream& os) const {
    // TO IMPLEMENT
}

void app::import(const engine::app::delta& gg, const engine::pow_t& w) {
    // TO IMPLEMENT
}

void us::gov::loan::local_delta::to_stream(ostream& os) const {
    b::to_stream(os);
}

void us::gov::loan::local_delta::from_stream(istream& is) {
    b::from_stream(is);
}

void us::gov::loan::delta::to_stream(ostream& os) const {
    b::b1::to_stream(os);

}

delta* us::gov::loan::delta::from_stream(istream& is) {
    delta* g=new delta();
    static_cast<b*>(g)->from_stream(is);
    return g;
}

string app::shell_command(const string& cmdline) {
    ostringstream os;
    istringstream is(cmdline);
    string cmd;
    is >> cmd;

    if (cmd=="hello") {
        os << "loan shell. type h for help." << endl;
    }
    else if (cmd=="h" || cmd=="help") {
        os << "loan shell." << endl;
        os << "h|help              Shows this help." << endl;
        os << "p|policies [id vote]          ." << endl;
        os << "exit                Exits this app and returns to parent shell." << endl;
        os << "" << endl;
    }
    else if (cmd=="p" || cmd=="policies") {
        int n=-1;
        double value;
        is >> n;
        is >> value;
        if (n>=0 && n<policies_traits::num_params) {
            lock_guard<mutex> lock(mx_policies);
            policies_local[n]=value;
        }
        else {
            os << "parameter " << n << " not found" << endl;
        }
        dump_policies(os);
    }
    else if (cmd=="exit") {
    }
    else {
        os << "Unrecognized command" << endl;
    }
    return os.str();
}

void app::dump_policies(ostream& os) const {
    lock_guard<mutex> lock(mx_policies);
    os << policies_traits::num_params << " consensus variables:" << endl;

    for (int i=0; i<policies_traits::num_params; ++i) {
        os << "  " << i << ". " << policies_traits::paramstr[i]
           << " [avg] consensus value: " << policies[i]
           << " local value:" << policies_local[i] << endl;
    }
}

