/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#ifndef GOV_SOCKET_CLIENTS_T_H_
#define GOV_SOCKET_CLIENTS_T_H_

#include <unordered_map>
#include <unordered_set>
#include <iostream>
#include <vector>
#include <mutex>

#include "client.h"

namespace us { namespace gov { namespace socket {
using namespace std;

/*!
 * \class clients_t
 *
 * \brief Hash tables of client objects
 *
 * Manages the client by providing different hash tables, one for each
 * different status a client can be: Active/Wait/Remove/Attic
 */
class clients_t: public unordered_map<int,client*> {
private:
    class attic_t: public unordered_set<client*> {
    public:
        ~attic_t();
        void add(client*);
        void purge();
    };

    class wait: public unordered_set<client*> {
    public:
        ~wait();
        void add(client* c);
        bool remove(client* c);
        bool find(const client& c) const;
        void dump(ostream& os) const;

        mutable mutex m_mx;
    };

    class rmlist: public unordered_set<client*> {
    public:
        ~rmlist();
        bool add(client* c);
        void dump(ostream& os) const;

        mutable mutex m_mx;
    };

    mutable mutex m_mx_active;
    mutable mutex m_mx;
    vector<client*> m_active;
    rmlist m_wremove;
    attic_t m_attic;
    wait m_wadd;

public:

    /** @brief Default constructor
     *
     */
    clients_t()=default;

    /** @brief Disable copy constructor
     *
     */
    clients_t(const clients_t& other)=delete;

    /** @brief Disable assignment operator
     *
     */
    clients_t& operator=(const clients_t&) =delete;

    /** @brief Destructor. Clean up the containers and delete clients.
     *
     */
    ~clients_t();

    /** @brief Resize and move clients in the right container based on
     *         their status
     *  @return vector of socket identifiers of clients that are active
     *          and ready to process new work
     *
     *  Clients that are in the 'remove' list will be moved in the 'attic'
     *  container and the clients in the 'wait' list will be promoted in the
     *  'active' list. The clients in the attic will be purged.
     *
     */
    vector<int> update();


    /** @brief List of active clients
     *  @return vector of pointer to client
     *
     */
    vector<client*> active() const;

    /** @brief Add a new client to the waiting list
     *  @param[in] cl client pointer to add
     *  @paramn[in] wakeupselect write a byte on the loopback
     *              socket to notify that the file descriptor is available
     *  @return Void
     *
     */
    void add(client* cl, bool wakeupselect=true);

    /** @brief prints information about the clients and the
     *         hash tables they belong to
     *  @param[in] os output stream to write to
     *  @return Void
     *
     */
    void dump(ostream& os) const;

    /** @brief add client in the remove list
     *  @param[in] cl client pointer to add
     *  @return Void
     *
     *  It automatically wakeup the select in the server by notifying that the
     *  socket file descriptor is available
     *
     */
    void remove(client* cl);

    /** @brief wakeup select
     *  @return Void
     *
     *  It writes a single bytes on the loopback socket just to wakeup the select
     *
     */
    void read_sockets();

    /** @brief move to the attic container clients that are candidatre for removal
     *  @return Void
     *
     */
    void shrink();

    /** @brief move clients in the waiting list in the active one
     *  @return Void
     *
     */
    void grow();

    /** @brief public member that instantiate a loopback client
     *
     */
    client m_locli; //loopback
};

}}}

#endif /* GOV_SOCKET_CLIENTS_T_H_ */
