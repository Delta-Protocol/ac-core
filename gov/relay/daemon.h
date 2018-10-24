/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#ifndef US_GOV_RELAY_DAEMON_H
#define US_GOV_RELAY_DAEMON_H

#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <cstdint>
#include <vector>
#include <mutex>

#include "us/gov/socket/datagram.h"
#include "us/gov/peer/daemon.h"
#include "us/gov/id/daemon.h"
#include "peer_t.h"

namespace us { namespace gov { namespace relay {
using namespace std;
using socket::datagram;

/*!
 * \class daemon
 *
 * \brief Relay datagrams to the neighbour peers
 *
 */
class daemon: public peer::daemon {
protected:
    virtual vector<peer_t*> get_nodes()=0;
    virtual bool process_evidence(datagram*)=0;
    virtual bool process_work(socket::peer_t *c, datagram*d) override;

public:
    /** @brief Alias for set of datagrams of type evidence
     *
     */
    typedef unordered_set<datagram::hash_t> evidences_t;

    /** @brief Default constructor. Create empty set of evidences
     *
     */
    daemon();

    /** @brief Constructor. Initialise base class and create empty set of evidences
     *  @param[in] port which port the server is listening too
     *  @param[in] edges number of neighbour (corresponds to size of thred pool)
     */
    daemon(uint16_t port, uint16_t edges);

    /** @brief Virtual destructor. Delete evidence set
     *
     */
    virtual ~daemon();

    /** @brief Print information about the size of the evidences set
     *  @param[in] os output stream to print to
     *
     */
    void dump(ostream&os) const;

    /** @brief Send datagram to neighbour peers
     *  @param[in] g datagram to relay
     *  @param[in] exclude pointer of peer to exclude (normally exclude the sender)
     *  @return Void
     *
     */
    using peer::daemon::send;
    void send(const datagram& g, socket::peer_t* exclude=0);

    /** @brief Retrieve set of evidence datagrams
     *  @return pointer to evidences set
     *
     *  It returns the set of evidences and initialise the one hold by
     *  this object with a new empty one
     *
     */
    evidences_t* retrieve_evidences(); //caller must take the lock

    /** @brief Returns the mutex to lock on evidences. The caller should hold the mutex
     *         when retrieving evidence datagrams
     * @return reference to mutex to lock on evidences
     *
     */
    mutex& get_evidences(){
        return m_mx_evidences;
    }
   
private:
    mutable mutex m_mx_evidences;
    evidences_t* m_evidences;
};

}}}

#endif

