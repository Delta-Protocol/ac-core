#ifndef USGOV_d99aa614ace853bcae148b472d209a4651c9b4bcdd2773e1d531e579bde9ba9c
#define USGOV_d99aa614ace853bcae148b472d209a4651c9b4bcdd2773e1d531e579bde9ba9c

#include <stdint.h>
#include <us/gov/relay/protocol.h>

namespace us { namespace gov {
namespace protocol {

	static constexpr uint16_t cash_base{700};
	static constexpr uint16_t cash_tx{((cash_base+1)<<2)+evidence_suffix};
	static constexpr uint16_t cash_query{((cash_base+2)<<2)+query_suffix};
	static constexpr uint16_t cash_response{((cash_base+3)<<2)+query_suffix};
}}
}

#endif
