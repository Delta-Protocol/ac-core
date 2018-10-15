#ifndef USGOV_2d8af251450aa79ce1d82cd9d9330072930013905de9945ca29228083ef3eee4
#define USGOV_2d8af251450aa79ce1d82cd9d9330072930013905de9945ca29228083ef3eee4

#include <atomic>

#include "client.h"

namespace us { namespace gov { namespace socket {
using namespace std;

class daemon;

class peer_t: public client {
public:

    peer_t();
    peer_t(int sock);
    virtual ~peer_t();

    void process_pong();
	bool is_slow() const; //take a lock before call

    bool ping();
    bool process_work(datagram* d);

    void dump(ostream& os) const {}
    virtual void dump_all(ostream& os) const override {
        dump(os);
        client::dump_all(os);
    }

    virtual void disconnect() override;
    virtual void on_detach() override;

    daemon* parent{0};
};

}}}

#endif

