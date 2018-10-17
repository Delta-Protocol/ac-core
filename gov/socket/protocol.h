/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#ifndef US_GOV_SOCKET_PROTOCOL_H
#define US_GOV_SOCKET_PROTOCOL_H

#include <inttypes.h>

namespace us { namespace gov { namespace protocol {
static constexpr uint16_t protocol_suffix{0};
static inline bool is_node_protocol(const uint16_t& i) { return (i&3)==protocol_suffix; }

#include <us/api/apitool_generated__protocol_gov_socket_cpp>

}}}

#endif
