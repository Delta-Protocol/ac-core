#ifndef USGOV_053e7a3036e689162036fca90f15499d3546c78380e3221c2c49c33806c112b2
#define USGOV_053e7a3036e689162036fca90f15499d3546c78380e3221c2c49c33806c112b2

#include <stdint.h>
#include <us/gov/socket/protocol.h>

namespace us{ namespace gov{ namespace protocol{
    static constexpr uint16_t engine_base{600};
    static constexpr uint16_t local_deltas{((engine_base+0)<<2)+protocol_suffix};
    static constexpr uint16_t vote_tip{((engine_base+1)<<2)+protocol_suffix};
    static constexpr uint16_t sysop{((engine_base+4)<<2)+protocol_suffix};
}}}

#endif
