#ifndef USGOV_e71c29d5f8c07b8000435dfe6a5f1e49605b1b2041dad14a3ac898464ed5dd49
#define USGOV_e71c29d5f8c07b8000435dfe6a5f1e49605b1b2041dad14a3ac898464ed5dd49

#include <array>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>

#include "us/gov/auth/peer_t.h"

namespace us { namespace gov { namespace peer {
using namespace std;

class daemon;

class peer_t: public auth::peer_t {
public:

    using datagram=socket::datagram;
    enum stage_t {
        disconnected=0,
        connected,
        exceed_latency,
        service,
        disconnecting,
        num_stages
    };

    constexpr static
    array<const char*,num_stages> stagestr={"disconnected",
                                            "connected",
                                            "latency",
                                            "service",
                                            "disconnecting"};

    peer_t(int sock);
    virtual ~peer_t();
    virtual string connect(const string& host, uint16_t port, bool block=false) override;
    virtual void disconnect() override;

    void dump(ostream& os) const;
    virtual void dump_all(ostream& os) const override {
        dump(os);
        auth::peer_t::dump_all(os);
    }

    chrono::steady_clock::time_point sent_ping;
    chrono::steady_clock::time_point since;
    stage_t stage;
};

}}}

#endif

