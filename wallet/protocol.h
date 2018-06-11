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
	static constexpr uint16_t pair_query{wallet_base+10};
	static constexpr uint16_t unpair_query{wallet_base+11};
	static constexpr uint16_t list_devices_query{wallet_base+12};

	static constexpr uint16_t nova_load{wallet_base+6}; //item is associated with a compartiment
	static constexpr uint16_t nova_reading{wallet_base+7}; // compartiment status
	static constexpr uint16_t nova_unload{wallet_base+8}; //item is unloaded from its compartiment

	static constexpr uint16_t response{wallet_base+0};

}
}}


#endif
