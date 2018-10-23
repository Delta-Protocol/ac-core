#ifndef US_GOV_ENGINE_TCP_ADDR_H
#define US_GOV_ENGINE_TCP_ADDR_H

#include "addr_t.h"

namespace us{ namespace gov{ namespace engine{
using namespace std;

class tcp_addr: public addr_t {
public:
    typedef addr_t b;
    virtual void to_stream(ostream& os) const override;
    static addr_t* from_stream(istream& is);

    static constexpr char m_id{'A'};

private:
    string m_address;
    uint16_t m_port;
};

}}}

#endif



