#include "tcp_addr.h"

using namespace std;
using namespace us::gov::engine;

constexpr char tcp_addr::m_id;

void tcp_addr::to_stream(ostream& os) const {
    os << m_id << ' ' << m_address << ' ' << m_port;
}

tcp_addr::b* tcp_addr::from_stream(istream& is) {
    auto i=new tcp_addr();
    is >> i->m_address;
    is >> i->m_port;
    return i;
}

