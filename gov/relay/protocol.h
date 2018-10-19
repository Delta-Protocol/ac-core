/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#ifndef US_GOV_RELAY_PROTOCOL_H
#define US_GOV_RELAY_PROTOCOL_H

#include <inttypes.h>

namespace us { namespace gov { namespace protocol {

/** @brief Protocol offset for relay layer
 *
 */
static constexpr uint16_t relay_base{400};

/** @brief Offset for datagram type evidence
 *
 */
static constexpr uint16_t evidence_suffix{1};

/** @brief Offset for datagram type query
 *
 */
static constexpr uint16_t query_suffix{2};

/** @brief Check if the given protocol is of type evidence
 *  @param[in] i protocol value
 *  @return true if this datagram is of type evidence, false otherwise
 *
 */
static inline bool is_evidence(const uint16_t& i) { return (i&3)==evidence_suffix; }

/** @brief Check if the given protocol is of type query
 *  @param[in] i protocol value
 *  @return true if this datagram is of type query, false otherwise
 *
 */
static inline bool is_app_query(const uint16_t& i) { return (i&3)==query_suffix; }

}}}

#endif
