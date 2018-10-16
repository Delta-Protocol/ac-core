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
    m_pool=new loan::local_delta();
    m_policies_local=m_policies;
}

app::~app() {
    delete m_pool;
}

void app::add_policies() {
    lock_guard<mutex> lock(m_mx_pool);
    lock_guard<mutex> lock2(m_mx_policies);
    for (int i=0; i<policies_traits::num_params; ++i)
        (*m_pool)[i] = m_policies_local[i];
}

engine::app::local_delta* app::create_local_delta() {
    add_policies();
    lock_guard<mutex> lock(m_mx_pool);
    auto full=m_pool;
    m_pool=new loan::local_delta();
    return full; //send collected transactions to the network
}

void app::dbhash(hasher_t&) const {
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
    base_t::b1::to_stream(os);

}

delta* us::gov::loan::delta::from_stream(istream& is) {
    delta* g=new delta();
    static_cast<base_t*>(g)->from_stream(is);
    return g;
}

uint64_t us::gov::loan::delta::merge(engine::app::local_delta* other0) {
    local_delta* other=static_cast<local_delta*>(other0);
    auto val=other->m_fees;
    base_t::merge(other0);
    return val;
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
            lock_guard<mutex> lock(m_mx_policies);
            m_policies_local[n]=value;
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
    lock_guard<mutex> lock(m_mx_policies);
    os << policies_traits::num_params << " consensus variables:" << endl;

    for (int i=0; i<policies_traits::num_params; ++i) {
        os << "  " << i << ". " << policies_traits::paramstr[i]
           << " [avg] consensus value: " << m_policies[i]
           << " local value:" << m_policies_local[i] << endl;
    }
}

