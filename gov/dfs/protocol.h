#ifndef USGOV_97f4ca5009925205caddff033edb001948dc6b58d5c2e82468ef782c1a30b756
#define USGOV_97f4ca5009925205caddff033edb001948dc6b58d5c2e82468ef782c1a30b756

#include <inttypes.h>
#include <us/gov/relay/protocol.h>

namespace us { namespace gov {
namespace protocol {

	static constexpr uint16_t dfs_base{500};
	static constexpr uint16_t file_request{((dfs_base+0)<<2)+query_suffix};
	static constexpr uint16_t file_response{((dfs_base+1)<<2)+query_suffix};
}
}}

#endif
