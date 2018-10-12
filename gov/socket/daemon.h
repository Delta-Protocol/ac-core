#ifndef USGOV_f06019c0635c00639ca65f74f67bed1a4db6bb3d506993ed4bac336c93eb48bb
#define USGOV_f06019c0635c00639ca65f74f67bed1a4db6bb3d506993ed4bac336c93eb48bb

#include <cstdint>
#include <iostream>

#include "ctpl_stl.h"
#include "server.h"

namespace ctpl {
class thread_pool;
} /* namespace ctpl */

namespace us { namespace gov { namespace socket {
using namespace std;

class peer_t;

class daemon: public server {

public:

    daemon();
    daemon(uint16_t port, int edges);
    virtual ~daemon();

    void run();
    virtual void on_finish() override;
    virtual client* create_client(int sock) override;
    virtual void receive_and_process(client*) override;
    virtual void attach(client*,bool wakeupselect=true) override;

    virtual void daemon_timer() {}
    virtual bool process_work(peer_t *c, datagram*d);

    void process_work(peer_t *c);
    void send(int num, peer_t* exclude, datagram* d);

    void dump(ostream& os) const;

    ctpl::thread_pool* m_pool{0};

    struct perf_t {
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

    perf_t perf;
};

}}}

#endif

