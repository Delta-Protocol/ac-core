/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#ifndef US_GOV_SOCKET_SERVER_H
#define US_GOV_SOCKET_SERVER_H

#include <atomic>
#include <mutex>
#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "us/gov/crypto/ec.h"
#include "us/gov/signal_handler.h"
#include "clients_t.h"

namespace us { namespace gov { namespace socket {
class datagram;

/*!
 * \class server
 *
 * \brief Manages incoming client connections
 *
 * Handles clients connections using three different containers based on the
 * status of the client. A client only exists in one container at the time.
 * The available lists are Wait/Active/Attic/Remove.
 * New incoming clients are added into a wait list until get work to process.
 * If new client becomes active it will be moved in the list of active clients.
 * Disconnected client will be added for candidate in the remove list.
 * When a client is not active it will be moved to the attic.
 *
 */
class server: public signal_handler::callback {
private:
    bool banned_throttle(const string& addr);

    uint16_t m_port;
    int m_sock {0};

protected:
    virtual void receive_and_process(client*c) { c->store_busy(false); };
    virtual void attach(client*,bool wakeupselect=true);
    virtual client* create_client(int sock);
    virtual void on_finish() override;

    clients_t m_clients;
    vector<client*> active() const { return m_clients.active(); }

public:
    /** @brief Default constructor
     *
     *  Initialise the listening port to 0
     */
    server();

    /** @brief Constructor with specified port number
     *  @param[in] port the port number of the server
     *
     *  Initialise the listening port to the specified port
     */
    explicit server(uint16_t port);

    /** @brief Virtual destructor
     *
     */
    virtual ~server();

    /** @brief Create a new socket
     *  @param[in] port Port to bind socket to
     *  @return Socket identifier. If socket cannot be created returns 0
     *
     *  Create a new socket listening on a spcified port
     */
    int make_socket(uint16_t port);

    /** @brief Prints information
     *  @param[out] os output stream to print on
     *  @return Void
     *
     *  Print to the output stream information regarding the server and the
     *  clients managed by it.
     */
    void dump(ostream& os) const;

    /** @brief Run this instance of the server in an infinite loop
     *  @return Void
     *
     *  Setup server and accepts client connections. Establishes a new socket for
     *  each new client and manages the different lists a client can belong to.
     */
    void run();

    /** @brief Remove a client.
     *  @param[in] c pointer to client to remove. Must be not null.
     *  @return Void
     *
     *  Add a client in the remove list
     */
    virtual void detach(client* c);

    /** @brief Alias for public key type used throughout the project
     *
     */
    typedef crypto::ec::keys::pub_t pub_t;
};

}}} /* US_GOV_SOCKET_SERVER_H */

#endif

