/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#ifndef US_GOV_SOCKET_DATAGRAM_H
#define US_GOV_SOCKET_DATAGRAM_H

#include <iostream>
#include <stddef.h>
#include <cstdint>
#include <string>
#include <vector>

#include "us/gov/crypto/ripemd160.h"

namespace us { namespace gov { namespace socket {
using namespace std;

/*!
 * \class datagram
 *
 * \brief Object used for communications between peers
 *
 * It is represented as buffer of bytes with a header made of 6 bytes
 * and a payload which can be upto 100kB.
 */
class datagram: public vector<uint8_t> {
private:
    size_t m_dend;
    static string service_str(uint16_t svc);
    inline string service_str() const { return service_str(service); }

    uint32_t decode_size() const;
    uint16_t decode_service() const;
    void encode_size(uint32_t);
    void encode_service(uint16_t);

public:

    /** @brief Alias for ripemd160 used throughout the project
     *
     */
    typedef crypto::ripemd160 hasher_t;

    /** @brief Alias for ripemd160 value_type object for conversions
     * 	       and comparision operations
     *
     */
    typedef hasher_t::value_type hash_t;

    /** @brief Header size of a datagram
     *
     */
    static constexpr size_t h{6};

    /** @brief Maximum size of a datagram
     *
     */
    static constexpr size_t maxsize{100000};

    /** @brief Default constructor. Initialise the actual size
     *        (running position) of the datagram to zero
     *
     */
    datagram();

    /** @brief Construct a datagram with the service number
     *         encoded in the header.
     *	@params[in] service type of datagram to create
     *
     *	Service numbers are specified in the protocol namespace of each module
     */
    explicit datagram(uint16_t service);

    /** @brief Construct a datagram with the service number and simple payload
     *  @param[in] service type of datagram to create
     *  @param[in] payload 2 bytes payload
     *
     */
    datagram(uint16_t service, uint16_t payload);

    /** @brief Construct a datagram with the service number and string payload
     *  @param[in] service identifier
     *  @param[in] payload string of characters
     *
     */
    datagram(uint16_t service, const string& payload);

    /** @brief Construct a datagram with the service number and string payload
     *  @return true if datagram is not empty and received completely
     */
    bool completed() const;

    /** @brief Compute ripemd160 hash of the datagram (head and payload)
     *  @return ripemd160 hash object
     */
    hash_t compute_hash() const;

    /** @brief Compute ripemd160 hash of the datagram payload only
     *  @return ripemd160 hash object
     */
    hash_t compute_payload_hash() const;

    /** @brief Send this datagram onto a specific socket
     *  @param[in] sock socket identifier
     *  @return string with error or success message
     *
     *  The following messages are returned:
     *  - 'Error. Datagram is too big.' if the size is greater than 100KB
     *  - 'Error. Connection is closed.' if the socket identifier is 0
     *  - 'Error. Failure writting to socket.' if the datragram cannot
     *     be written to the socket the message
     *  - 'Error. Unexpected returning size while wrtting to socket' if the
     *     written size is different than the size of the datagram the message
     *  -  An empty string for success.
     */
    string sendto(int sock) const;

    /** @brief Receive and populate this datagram from a specific socket
     *  @param[in] sock socket identifier
     *  @return string with error or success message
     *
     *	It attempts to read the header and the payload.
     *	The following messages are returned:
     *	- 'Timeout waiting for data from peer.' if the header/payload cannot
     *	   be received in case of timeout or 'Connection ended by peer.' otherwise.
     *  - 'Error. Incoming datagram exceeded size limit.' if the datagram
     *     is too large (greater than 100KB)
     *  - 'Error. Connection is closed.' if the socket identifier is 0
     *  - Empty string if there is more data to receive or the whole datagram
     *     has arrived
     */
    string recvfrom(int sock);

    /** @brief Convert datagram payload in a string
     *  @return string version of datragram (null terminated string).
     *          Empty string is the payload is less than 2 bytes.
     */
    string parse_string() const;

    /** @brief Fetch two bytes datagram payload
     *  @return uint16_t two bytes payload. If less than two
     *          bytes are available returns 0
     */
    uint16_t parse_uint16() const;

    /** @brief Dump datagram payload information
     *  @return Void
     */
    void dump(ostream&) const;

    /** @brief Public member to identify the service of this datagram
     */
    uint16_t service;
};

}}}

#endif

