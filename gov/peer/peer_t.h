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
using datagram=socket::datagram;

class daemon;

class peer_t: public auth::peer_t {

protected:

    virtual void dump_all(ostream& os) const override;

public:

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

    virtual void disconnect() override;
    virtual string connect(const string& host,
                           uint16_t port,
                           bool block=false) override;

    void dump(ostream& os) const;

    chrono::steady_clock::time_point m_sent_ping;
    chrono::steady_clock::time_point m_since;
    stage_t m_stage;
};

}}}

#endif

