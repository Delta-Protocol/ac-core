/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#ifndef US_GOV_PEER_DAEMON_H
#define US_GOV_PEER_DAEMON_H

#include <unordered_map>
#include <unordered_set>
#include <stddef.h>
#include <iostream>
#include <sstream>
#include <cstdint>
#include <string>
#include <vector>

#include "us/gov/auth/daemon.h"
#include "peer_t.h"

namespace us { namespace gov { namespace peer {
using namespace std;
using socket::datagram;

/*!
 * \class daemon
 *
 * \brief Manages peers. Provides interface to the classes that inherits from it
 *        to purge or add new peers and send datagrams
 *
 */
class daemon: public auth::daemon {
private:
    uint16_t m_edges;

    class peers_t: public vector<peer_t*> {
    public:
        size_t asize() const {
            size_t n=0;
            for (auto i:*this) if (i!=0) ++n;
            return n;
        }

        void dump(ostream&os) const {
            for (auto i:*this) {
                i->dump(os);
                os << endl;
            }
        }
    };

    peers_t adjust_peer_number();
    peers_t::iterator oldest(peers_t& v) const;

    void purge_excess(peers_t& a);
    void purge_peers(peers_t& a);
    void purge_slow(peers_t&a);
    void add_peers(peers_t& a);

    void check_latency(const peers_t&);
    vector<peer_t*> in_service(const peers_t& a) const;

protected:
    virtual void daemon_timer() override;
    virtual string get_random_peer(const unordered_set<string>& exclude) const=0;
    peers_t active() const;

public:

    /** @brief Default constructor. Initialise the number of peers to zero
     *
     */
    daemon();

    /** @brief Constructor.
     *  @param[in] port port of the server to connect to
     *  @param[in] edges size of the threadpool representing number
     *             of neighbour nodes
     *
     * It calls the constructor of the base class and initialise the port
     * of the server to connect to and the size of the threadpool (edges) which
     * corresponds to the number of neighbour nodes
     *
     */
    daemon(uint16_t port, uint16_t edges);

    /** @brief Virtual destructor
     *
     */
    virtual ~daemon();

    /** @brief Dump information regarding this peer manager
     *  @param[out] os output stream to print to
     *  @return Void
     *
     */
    void dump(ostream& os) const;

    /** @brief Send a datagram to a number of the neighbour peers
     *  @param[in] num number of neighbours to send the message to
     *  @param[in] exclude send datagram to all but this one
     *  @param[in] d datagram to send
     *  @return Void
     *
     *  If num is zero then it sends the datagram to all the neighbours. Otherwise
     *  it sends the datagram to 'num' peers. The list of peers is always randomly
     *  reshuffled so datagrams are not sent to the same peers.
     *
     */
    void send(int num, peer_t* exclude, datagram* d);

    /** @brief Get list of active clients that are in 'service" statuse
     *  @return vector of pointer to in 'service' peers
     *
     */
    vector<peer_t*> in_service() const;
};

}}}

#endif

