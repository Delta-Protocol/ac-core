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

class peer_t: public client {
private:
    void process_pong();
    void dump(ostream& os) const {}

protected:
    virtual void dump_all(ostream& os) const override;
    virtual void on_detach() override;

public:
    peer_t();
    peer_t(int sock);
    virtual ~peer_t();

    bool ping();
    bool process_work(datagram* d);

    virtual void disconnect() override;

    daemon* m_parent{0};
};

}}}

#endif

