#ifndef USGOV_9bf343e8ad5f89ce96b35e9e287ad85205c61b8bd324f693e11d2b89c55a1930
#define USGOV_9bf343e8ad5f89ce96b35e9e287ad85205c61b8bd324f693e11d2b89c55a1930

#include <inttypes.h>

namespace us { namespace gov {
namespace protocol {

    static constexpr uint16_t protocol_suffix{0};
    static inline bool is_node_protocol(const uint16_t& i) { return (i&3)==protocol_suffix; } 

    static constexpr uint16_t socket_base{100};
    static constexpr uint16_t ping{((socket_base+1)<<2)+protocol_suffix};
    static constexpr uint16_t pong{((socket_base+2)<<2)+protocol_suffix};

}
}}

#endif
