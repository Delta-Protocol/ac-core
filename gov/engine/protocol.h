#ifndef US_GOV_ENGINE_PROTOCOL_H
#define US_GOV_ENGINE_PROTOCOL_H

#include <stdint.h>
#include <us/gov/socket/protocol.h>

namespace us{ namespace gov{ namespace protocol{
    static constexpr uint16_t engine_base{600};
    static constexpr uint16_t local_deltas{((engine_base+0)<<2)+protocol_suffix};
    static constexpr uint16_t vote_tip{((engine_base+1)<<2)+protocol_suffix};
    static constexpr uint16_t sysop{((engine_base+4)<<2)+protocol_suffix};
}}}

#endif
