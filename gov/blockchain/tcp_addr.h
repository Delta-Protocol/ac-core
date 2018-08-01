#ifndef USGOV_895b787d4a6c5de6e648b2f1189f56696b74ef9ec9463cc13f1cc1f6e6636d6e
#define USGOV_895b787d4a6c5de6e648b2f1189f56696b74ef9ec9463cc13f1cc1f6e6636d6e

#include "addr_t.h"

namespace us{ namespace gov {
namespace blockchain {
using namespace std;

        struct tcp_addr: addr_t {
            typedef addr_t b;

            static constexpr char id{'A'};
            virtual string to_stream(ostream& os) const override;
            static addr_t* from_stream(istream& is);
            static addr_t* from_stream_prev(istream& is);

            string address;
            uint16_t port;
        };


}
}}

#endif



