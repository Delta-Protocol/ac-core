#ifndef USGOV_f7ece82349b5b34e6f48f4dd802ec8765a675855c2e34bc29f2fdc5cc28ed4ba
#define USGOV_f7ece82349b5b34e6f48f4dd802ec8765a675855c2e34bc29f2fdc5cc28ed4ba

#include <stdint.h>
#include <us/gov/peer/protocol.h>

namespace us { namespace gov {
namespace protocol {
	static constexpr uint16_t id_base{200};
	static constexpr uint16_t id_request{((id_base+0)<<2)+protocol_suffix};
	static constexpr uint16_t id_peer_challenge{((id_base+1)<<2)+protocol_suffix};
	static constexpr uint16_t id_challenge_response{((id_base+2)<<2)+protocol_suffix};
	static constexpr uint16_t id_peer_status{((id_base+3)<<2)+protocol_suffix};
}
}
}

#endif
