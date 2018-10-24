#ifndef US_GOV_CASH_PROTOCOL_H
#define US_GOV_CASH_PROTOCOL_H

#include <stdint.h>
#include <us/gov/relay/protocol.h>

namespace us{ namespace gov{ namespace protocol{
    static constexpr uint16_t cash_base{700};
    static constexpr uint16_t cash_tx{((cash_base+1)<<2)+evidence_suffix};
    static constexpr uint16_t cash_query{((cash_base+2)<<2)+query_suffix};
    static constexpr uint16_t cash_response{((cash_base+3)<<2)+query_suffix};
}}}

#endif
