/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#ifndef US_GOV_PEER_PEER_T_H
#define US_GOV_PEER_PEER_T_H

#include <iostream>
#include <cstdint>
#include <string>
#include <chrono>
#include <array>

#include "us/gov/auth/peer_t.h"

namespace us { namespace gov { namespace peer {
using namespace std;
using datagram=socket::datagram;
class daemon;

/*!
 * \class peer_t
 *
 * \brief Manages all peer related operations and its status.
 *        It inherits from auth::peer_t which handles all the authentication
 *        of the peer for communicating on the network
 *
 */
class peer_t: public auth::peer_t {
protected:
    virtual void dump_all(ostream& os) const override;

public:

    /*! Enumerates peer status */
    enum stage_t {
        disconnected=0,
        connected,
        exceed_latency,
        service,
        disconnecting,
        num_stages
    };

    /** @brief Mapping enumerator stage_t to string
     *
     */
    constexpr static
    array<const char*,num_stages> stagestr={"disconnected",
                                            "connected",
                                            "latency",
                                            "service",
                                            "disconnecting"};

    /** @brief Init the peer with the given socket. Default status is in serivce.
     *  @param sock socket identifier
     *
     */
    explicit peer_t(int sock);

    /** @brief Virtual destructor
     *
     */
    virtual ~peer_t();

    /** @brief Disconnect peer from socket.
     *  @return Void
     *
     *  It marks the status of the peer as "disconnecting" and calls the base
     *  class disconnect method.
     */
    virtual void disconnect() override;

    /** @brief Connect the peer to a new socket
     *  @param[in] host ip address to connect the peer to
     *  @param[in] port allowed port to connect the peer to
     *  @param[in] block flag set property of the open file descriptor as blocking
     *  @return string containing error message or empty string if successfully
     *          connected to the socket
     *  @see socket::connect
     *
     *  It calls the base class connect methods and marks the stage as in "service".
     */
    virtual string connect(const string& host,
                           uint16_t port,
                           bool block=false) override;

    /** @brief Dump peer information
     *  @param[out] os output stream to print to
     *  @return Void
     *
     */
    void dump(ostream& os) const;

    chrono::steady_clock::time_point m_sent_ping;
    chrono::steady_clock::time_point m_since;
    stage_t m_stage;
};

}}}

#endif
