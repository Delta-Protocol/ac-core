#ifndef US_GOV_ID_DAEMON_H
#define US_GOV_ID_DAEMON_H

#include "us/gov/socket/daemon.h"

#include <us/gov/crypto.h>

namespace us{ namespace gov{ namespace id{

using namespace std;

class daemon: public socket::daemon {
public:
    typedef crypto::ec::keys keys;

    daemon(){}
    daemon(uint16_t port, uint16_t edges);
    virtual ~daemon(){}

    virtual socket::client* create_client(int sock) override;
    void dump(ostream& os) const;

    virtual bool process_work(socket::peer_t *p, socket::datagram* d) override;
    virtual const keys& get_keys() const=0;
    bool is_duplicate(const pub_t&) const;
};

}}}

#endif

