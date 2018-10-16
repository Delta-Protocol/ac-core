#include <iomanip>
#include <sstream>
#include <tuple>

#include "peer_t.h"
#include "daemon.h"
#include "protocol.h"

using namespace us::gov::peer;

using namespace std;
using namespace chrono_literals;

constexpr array<const char*,peer_t::num_stages> peer_t::stagestr;

peer_t::peer_t(int sock): auth::peer_t(sock), m_stage(service) {
}

peer_t::~peer_t() {
}

string peer_t::connect(const string& host, uint16_t port, bool block) {
    auto r=auth::peer_t::connect(host,port,block);
    if (r.empty()) {
        m_stage=service;
    }
    return r;
}

void peer_t::disconnect() {
     m_stage=disconnecting;
     auth::peer_t::disconnect();
}

void peer_t::dump(ostream& os) const {
    os << this << "peer stage: " << stagestr[m_stage] << endl;
}

void peer_t::dump_all(ostream& os) const {
    dump(os);
    auth::peer_t::dump_all(os);
}

namespace {

template <typename Container, typename Fun>
void tuple_for_each(const Container& c, Fun fun) {
    for (auto& e : c)
        fun(std::get<0>(e), std::get<1>(e), std::get<2>(e));
}

string duration_str(chrono::milliseconds time) {
    using namespace std::chrono;
    using T = std::tuple<milliseconds, int, const char *>;

    constexpr T formats[] = {
            T{hours(1), 2, ""},
            T{minutes(1), 2, ":"},
            T{seconds(1), 2, ":"},
    };

    std::ostringstream o;
    tuple_for_each(formats, [&time, &o](auto denominator, auto width, auto separator) {
        o << separator << std::setw(width) << std::setfill('0') << (time / denominator);
        time = time % denominator;
    });

    return o.str();
}

string age(chrono::steady_clock::time_point time) {
    using namespace std::chrono;
    return duration_str(duration_cast<milliseconds>(steady_clock::now()-time));
}

} //namespace


