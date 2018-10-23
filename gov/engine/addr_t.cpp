#include "addr_t.h"
#include "tcp_addr.h"

using namespace std;
using namespace us::gov::engine;

addr_t* addr_t::from_stream(istream& is) {
    int n;
    is >> n;
    if (n==tcp_addr::m_id) 
        return tcp_addr::from_stream(is);
    return 0;
}

ostream& operator << (ostream& os, const addr_t& o) {
    o.to_stream(os);
    return os;
}

