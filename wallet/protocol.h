#ifndef USGOV_bcd39cb0e30ff62fab250e5e57bdcff3af1e5a4e8299d35f59dd942bd6f80adc
#define USGOV_bcd39cb0e30ff62fab250e5e57bdcff3af1e5a4e8299d35f59dd942bd6f80adc

#include <inttypes.h>

namespace us { namespace wallet {
namespace protocol {

	static constexpr uint16_t wallet_base{0};


	static constexpr uint16_t balance_query{wallet_base+1};
	static constexpr uint16_t dump_query{wallet_base+2};
	static constexpr uint16_t new_address_query{wallet_base+3};
	static constexpr uint16_t add_address_query{wallet_base+4};
	static constexpr uint16_t tx_make_p2pkh_query{wallet_base+5};
	static constexpr uint16_t tx_sign_query{wallet_base+6};
	static constexpr uint16_t tx_send_query{wallet_base+7};
	static constexpr uint16_t tx_decode_query{wallet_base+8};
	static constexpr uint16_t tx_check_query{wallet_base+9};


	static constexpr uint16_t response{wallet_base+0};

}
}}


#endif
