#ifndef USGOV_43feb86adb9469dd0ab43fe0ccfd2f37615284c9edef0b158f44276e8ee8270f
#define USGOV_43feb86adb9469dd0ab43fe0ccfd2f37615284c9edef0b158f44276e8ee8270f

#include <inttypes.h>

namespace us { namespace gov {
namespace protocol {
	static constexpr uint16_t protocol_suffix{0};
	static inline bool is_node_protocol(const uint16_t& i) { return (i&3)==protocol_suffix; } 

	static constexpr uint16_t peer_base{100};
	static constexpr uint16_t ping{((peer_base+1)<<2)+protocol_suffix};
	static constexpr uint16_t pong{((peer_base+2)<<2)+protocol_suffix};
}
}}

#endif
