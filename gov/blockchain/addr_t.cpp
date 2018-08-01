#include "addr_t.h"
#include "tcp_addr.h"

typedef us::gov::blockchain::addr_t c;
using namespace std;
using namespace us::gov::blockchain;

c* c::from_stream_prev(istream& is) {
    return from_stream(is);
}

c* c::from_stream(istream& is) { //X
    int n;
    is >> n;
    if (n==tcp_addr::id) return tcp_addr::from_stream(is);
    return 0;
}

ostream& operator << (ostream& os, const addr_t& o) {
        o.to_stream(os);
        return os;
}

