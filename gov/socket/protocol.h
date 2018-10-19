/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#ifndef US_GOV_SOCKET_PROTOCOL_H
#define US_GOV_SOCKET_PROTOCOL_H

#include <inttypes.h>

namespace us { namespace gov { namespace protocol {

/** @brief Offset to identify this protcol layer
 *
 */
static constexpr uint16_t protocol_suffix{0};

/** @brief Verify this protocol from an identifier
 *  @param[in] i identifier of the protocol to verify
 *  @return true if the service belongs to this layer
 *
 *  A datagram has an associated service number which is validated using this
 *  function to check at which layer of the protocol the datagram belongs to.
 *
 */
static inline bool is_node_protocol(const uint16_t& i) { return (i&3)==protocol_suffix; }

#include <us/api/apitool_generated__protocol_gov_socket_cpp>

}}}

#endif
