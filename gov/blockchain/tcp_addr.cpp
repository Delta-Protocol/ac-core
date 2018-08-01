#include "tcp_addr.h"

typedef us::gov::blockchain::tcp_addr c;
using namespace std;
using namespace us::gov::blockchain;

constexpr char c::id;

string c::to_stream(ostream& os) const {
   os << id << ' ' << address << ' ' << port;
}

c::b* c::from_stream_prev(istream& is) {
    return from_stream(is);
}

c::b* c::from_stream(istream& is) {
    auto i=new c();
    is >> i->address;
    is >> i->port;
    return i;
}

