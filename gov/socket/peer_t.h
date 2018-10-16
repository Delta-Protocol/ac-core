#ifndef USGOV_2d8af251450aa79ce1d82cd9d9330072930013905de9945ca29228083ef3eee4
#define USGOV_2d8af251450aa79ce1d82cd9d9330072930013905de9945ca29228083ef3eee4

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

