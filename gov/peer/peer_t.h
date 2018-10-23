/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#ifndef US_GOV_PEER_PEER_T_H
#define US_GOV_PEER_PEER_T_H

#include <iostream>
#include <cstdint>
#include <string>
#include <chrono>
#include <array>

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

    const chrono::steady_clock::time_point& get_since() const {
        return m_since;
    }

    const stage_t& get_stage() const {
        return m_stage;
    }

private:
    chrono::steady_clock::time_point m_sent_ping;
    chrono::steady_clock::time_point m_since;
    stage_t m_stage;
};

}}}

#endif
