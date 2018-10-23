#ifndef USGOV_895b787d4a6c5de6e648b2f1189f56696b74ef9ec9463cc13f1cc1f6e6636d6e
#define USGOV_895b787d4a6c5de6e648b2f1189f56696b74ef9ec9463cc13f1cc1f6e6636d6e

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



