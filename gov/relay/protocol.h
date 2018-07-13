#ifndef USGOV_349f061ef490458208c80bb6358d5c6be0b925001f7b531f3860b4e66eb4420d
#define USGOV_349f061ef490458208c80bb6358d5c6be0b925001f7b531f3860b4e66eb4420d

#include <inttypes.h>

namespace us { namespace gov {
namespace protocol {
	static constexpr uint16_t evidence_suffix{1};
	static constexpr uint16_t query_suffix{2};
	static inline bool is_evidence(const uint16_t& i) { return (i&3)==evidence_suffix; }
	static inline bool is_app_query(const uint16_t& i) { return (i&3)==query_suffix; }

        static constexpr uint16_t relay_base{300};
}
}}

#endif
