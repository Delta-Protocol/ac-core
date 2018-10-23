/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#ifndef US_GOV_SOCKET_DAEMON_H
#define US_GOV_SOCKET_DAEMON_H

#include <cstdint>
#include <iostream>

#include "ctpl_stl.h"
#include "server.h"

namespace us { namespace gov { namespace socket {
using namespace std;
class peer_t;

/*!
 * \class daemon
 *
 * \brief Daemon running the background until a termination signal is received
 *
 * It runs the server in a thread and mange the clients' incoming workload
 * by using a threadpool.
 */
class daemon: public server {
private:
    class perf_t {
    public:
        struct disconnections_t {
            struct datagram_t {
                uint64_t normal{0};
                uint8_t unknown_service{0};
            };

            struct thread_pool_t {
                uint64_t full{0};
            };

            datagram_t datagram;
            thread_pool_t thread_pool;
        };

        disconnections_t disconnections;
    };

private:
    perf_t m_perf;

    void process_work(peer_t *c);

protected:
    virtual void daemon_timer() {}
    virtual bool process_work(peer_t *c, datagram*d);

    virtual void on_finish() override;
    virtual client* create_client(int sock) override;
    virtual void receive_and_process(client*) override;
    virtual void attach(client*,bool wakeupselect=true) override;

    ctpl::thread_pool* m_pool{0};

public:
    /** @brief Default constructor. Initialise the threadpool size to zero
     *
     */
    daemon();

    /** @brief Construct the daemon
     *  @param[in] port server is listening a the specified port
     *  @param[in] edges initialise the size of the threadpool
     *
     */
    daemon(uint16_t port, int edges);

    /** @brief Virtual destructor
     *
     */
    virtual ~daemon();

    /** @brief Main loop that execute the daemon
     *  @return Void
     *
     *  It creates an istance of the server running in its own thread. The
     *  server manage the incoming clients connections and each client workload
     *  will will be added and managed by the threadpool defined in this
     *  class.
     *
     */
    void run();

    /** @brief Virtual destructor
     *  @param[in] os output stream to print information to
     *  @return Void
     *
     */
    void dump(ostream& os) const;
};

}}}

#endif

