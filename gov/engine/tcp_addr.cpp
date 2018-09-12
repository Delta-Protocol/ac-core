#include "tcp_addr.h"

typedef us::gov::engine::tcp_addr c;
using namespace std;
using namespace us::gov::engine;

constexpr char c::id;

void c::to_stream(ostream& os) const {
   os << id << ' ' << address << ' ' << port;
}

c::b* c::from_stream(istream& is) {
    auto i=new c();
    is >> i->address;
    is >> i->port;
    return i;
}

