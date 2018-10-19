/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#ifndef US_GOV_SOCKET_CLIENT_H
#define US_GOV_SOCKET_CLIENT_H

#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <cstdint>
#include <utility>
#include <atomic>
#include <mutex>
#include <string>

#include "datagram.h"

namespace us { namespace gov { namespace socket {
using namespace std;

/*!
 * \class client
 *
 * \brief Establish a connection on a socket and has functionalities to enable
 *        communication and exchange of datagrams
 *
 */
class client {
private:
    void init_sockaddr(struct sockaddr_in *name,
                       const char *hostname,
                       uint16_t port);

    string init_sock(const string& host,
                     uint16_t port,
                     bool block=false);

    mutable mutex m_mx;
    mutable string m_msg;

public:

    /** @brief Default constructor. Initialise socket identifier to zero
     *
     */
    client();

    /** @brief Constructor. Initialise socket with specified identifier
     *  @param[in] sock socket identifier
     *
     */
    explicit client(int sock);

    /** @brief Virtual destructor. It disconnects the client from the socket
     *
     */
    virtual ~client();

    /** @brief Tells if the client has connected to a socket
     *  @return true if it is connect, false otherwise
     *
     */
    inline bool connected() const { return m_sock!=0; }

    /** @brief Connect client to server
     *  @param[in] host address of the server to connect to
     *  @param[in] port of the server to connect to
     *  @param[in] block flag set property of the open file descriptor as blocking
     *  @return string with error message or empty string on success
     *
     *  The following error messages could be returned:
     *  - 'socket is non zero.' is the socket is already bound
     *  - 'Error. Timeout obtaining peername' if after 500ms the name of the
     *    peer cannot be obtained
     *  - 'Connection refused. errno=' server refuses connection and will
     *     print the error number
     *
     */
    virtual string connect(const string& host,
                           uint16_t port,
                           bool block=false);

    /** @brief Close the socket connection
     *  @return Void
     *
     */
    virtual void disconnect();

    /** @brief Action to take on disconnecting the client.
     *  @return Void
     *
     *  No implementation is provided in this context as not action is
     *  required but the peer that inherit from it could redefine this method
     *
     */
    virtual void on_detach() {}

    /** @brief Print connection information
     *  @param[out] os output stream to print to
     *  @return Void
     *
     */
    void dump(ostream& os) const;

    /** @brief Print connection information.
     *  @param[out] os output stream to print to
     *  @return Void
     *
     */
    virtual void dump_all(ostream& os) const { dump(os); }

    /** @brief Receive a datagram on the established socket connection
     *  @return pair with string which is empty on success or with an error
     *          message in case of failure and a pointer to the datagram
     *          just received
     *  @see datagram::recvfrom(int sock)
     *
     *  It calls the datagram method recvfrom
     */
    pair<string,datagram*> recv();

    /** @brief Polling to receive datagram of a specified type
     *  @param[in] expected_service protocol specifying the type of datagram
     *  @return pair with string which is empty on success or with an error
     *          message in case of failure and a pointer to the datagram
     *          just received
     *  @see client::recv()
     *
     *  It implemented in terms of client::recv().
     */
    pair<string,datagram*> recv(uint16_t expected_service);

    /** @brief Attempt to send a datagram and then receive one
     *  @param[in] d datagram do send
     *  @return pair with string which is empty on success or with an error
     *          message in case of failure and a pointer to the datagram
     *          just received
     *  @see client::send()
     *  @see client::recv()
     *
     *  It implemented in terms of client::send() and client::recv().
     */
    pair<string,datagram*> send_recv(datagram* d);

    /** @brief Attempt to send a datagram and then wait to receive one
     *         of a specific type
     *  @param[in] d datagram do send
     *  @param[in] expected_service type of datagram to wait for
     *  @return pair with string which is empty on success or with an error
     *          message in case of failure and a pointer to the datagram
     *          just received
     *  @see client::send()
     *  @see client::recv(uint16_t expected_service)
     *
     *  It implemented in terms of client::send() and
     *  client::recv(uint16_t expected_service).
     */
    pair<string,datagram*> send_recv(datagram* d,uint16_t expected_service);

    /** @brief Send a datagram using the socket established by this client
     *  @param[in] d datagram to send
     *  @return string with error message or empty string on success.
     *          In case of error the socket is closed hence the cliend disconnected
     *
     *  The following error message are returned:
     *  - 'Error. Sending datagram before connecting.' if socket is not connected
     *  - 'Error. Datagram is too big.' if datagram is too large (default 100KB)
     *  - 'Error. Connection is closed.' connection not available
     *  - 'Error. Failure writing to socket.' write operation failed
     *  - 'Error. Unexpected returning size while wrtting to socket' the datagram
     *    was only partially delivered.
     */
    string send(const datagram& d);

    /** @brief Send a datagram using the socket established by this client
     *  @param[in] d datagram to send
     *  @return string with error message or empty string on success.
     *          In case of error the socket is closed and the client disconnected
     *
     *  Overloaded version of send datagram accepting a pointer insted of reference
     *
     */
    string send(datagram* d);

    /** @brief Get client IP address
     *  @return string representing client IP address
     *
     */
    string address() const;

    /** @brief Flag that marks if the client is busy processing work
     *
     */
    atomic<bool> m_busy{false};

    /** @brief Peer name public attribute
     *
     */
    string m_addr;

    /** @brief Socket identifier public attribute
     *
     */
    int m_sock;
};

}}}

#endif

