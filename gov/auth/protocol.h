#ifndef USGOV_2889c3f0dfd9f8b536adeede941b07163e785c6f61b0d6a4fa7fd2e8561175d9
#define USGOV_2889c3f0dfd9f8b536adeede941b07163e785c6f61b0d6a4fa7fd2e8561175d9

#include <stdint.h>
#include <gov/peer/protocol.h>

namespace usgov {
namespace protocol {
	static constexpr uint16_t auth_base{200};
	static constexpr uint16_t auth_request{((auth_base+0)<<2)+protocol_suffix};
	static constexpr uint16_t auth_peer_challenge{((auth_base+1)<<2)+protocol_suffix};
	static constexpr uint16_t auth_challenge_response{((auth_base+2)<<2)+protocol_suffix};
	static constexpr uint16_t auth_peer_status{((auth_base+3)<<2)+protocol_suffix};
}
}

#endif
