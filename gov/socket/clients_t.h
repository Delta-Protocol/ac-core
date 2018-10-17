/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#ifndef GOV_SOCKET_CLIENTS_T_H_
#define GOV_SOCKET_CLIENTS_T_H_

#include <iostream>
#include <mutex>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "client.h"

namespace us { namespace gov { namespace socket {
using namespace std;

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
    clients_t() {}
    clients_t(const clients_t& other)=delete;
    ~clients_t();

    vector<int> update();
    vector<client*> active() const;

    void add(client* c, bool wakeupselect=true);
    void dump(ostream& os) const;
    void remove(client* c);
    void read_sockets();
    void shrink();
    void grow();

    client m_locli; //loopback
};

}}}

#endif /* GOV_SOCKET_CLIENTS_T_H_ */
