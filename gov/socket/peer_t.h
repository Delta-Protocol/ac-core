/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#ifndef US_GOV_SOCKET_PEER_T_H
#define US_GOV_SOCKET_PEER_T_H

#include <atomic>
#include "client.h"

namespace us { namespace gov { namespace socket {
using namespace std;
class daemon;

/*!
 * \class peer_t
 *
 * \brief Extends client functionalities
 *
 * It is initialised with the same socket as for the client. A node can have
 * more than one peer to process workload.
 *
 */
class peer_t: public client {
private:
    void process_pong();
    void dump(ostream& os) const {}

protected:
    virtual void dump_all(ostream& os) const override;
    virtual void on_detach() override;

public:
    /** @brief Default constructor
     *
     *  Initialise the base client socket with identifier 0
     */
    peer_t();

    /** @brief Constructor with specified socket identifier
     *  @param[in] sock the socket identifier for communicating
     *
     *  Initialise the base client socket with the indentifier provided
     */
    explicit peer_t(int sock);

    /** @brief Virtual destructor
     *
     */
    virtual ~peer_t();

    /** @brief Send a datagram ping over the established socket
     *
     */
    bool ping();

    /** @brief Process a datagram
     *  @param[in] dgram datagram to process
     *  @return true if work is processed, false otherwise
     *
     *  Based on the work to process it will send the appropriate
     *  datagram in response
     */
    bool process_work(datagram* dgram);

    /** @brief Disconnect peer
     *
     *  If the peer is the parent itself (the one who created the new
     *  neighbour client) then it will detach the client by moving it in
     *  the list of candidates for disconnection. Otherwise
     *  just close the socket directly.
     */
    virtual void disconnect() override;

    /** @brief Return member variable holding the pointer to the parent daemon
     *  @return member variable holding the pointer to the parent daemon
     *
     *  The parent is initialised when the client is created
     *  by the daemon (in socket layer) and it is used to identify who
     *  created the peer and access functionalities exposed by the parent only.
     */
    const daemon* get_parent() const {
        return m_parent;
    } 

    /** @brief Set member variable holding the pointer to the parent daemon
     *  @param[in] pointer to daemon
     */
    void set_parent(daemon* d){
        m_parent = d;
    } 
protected:
    daemon* m_parent{0};
};

}}}

#endif

