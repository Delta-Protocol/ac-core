#ifndef USGOV_9bf343e8ad5f89ce96b35e9e287ad85205c61b8bd324f693e11d2b89c55a1930
#define USGOV_9bf343e8ad5f89ce96b35e9e287ad85205c61b8bd324f693e11d2b89c55a1930

#include <inttypes.h>

namespace us { namespace gov {
namespace protocol {

    static constexpr uint16_t protocol_suffix{0};
    static inline bool is_node_protocol(const uint16_t& i) { return (i&3)==protocol_suffix; } 

#include <us/api/apitool_generated__protocol_gov_socket_cpp>

}
}}

#endif
