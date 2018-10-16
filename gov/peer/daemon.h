#ifndef USGOV_bf993116c1d89300316c0dd4fa2ef165d5b9128da89d33c44cfe50e95622fce8
#define USGOV_bf993116c1d89300316c0dd4fa2ef165d5b9128da89d33c44cfe50e95622fce8

#include <stddef.h>
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <sstream>

#include "us/gov/auth/daemon.h"
#include "peer_t.h"

namespace us { namespace gov { namespace peer {
using namespace std;
using socket::datagram;

class daemon: public auth::daemon {

private:

    uint16_t m_edges;

    class peers_t: public vector<peer_t*> {
    public:
        size_t asize() const {
            size_t n=0;
            for (auto i:*this) if (i!=0) ++n;
            return n;
        }

        void dump(ostream&os) const {
            for (auto i:*this) {
                i->dump(os);
                os << endl;
            }
        }
    };

    peers_t adjust_peer_number();
    peers_t::iterator oldest(peers_t& v) const;

    void purge_excess(peers_t& a);
    void purge_peers(peers_t& a);
    void purge_slow(peers_t&a);
    void add_peers(peers_t& a);

    void check_latency(const peers_t&);
    vector<peer_t*> in_service(const peers_t& a) const;

protected:

    virtual void daemon_timer() override;
    virtual string get_random_peer(const unordered_set<string>& exclude) const=0;
    peers_t active() const;

public:

    daemon();
    daemon(uint16_t port, uint16_t edges);
    virtual ~daemon();

    void dump(ostream& os) const;
    void send(int num, peer_t* exclude, datagram* d);
    vector<peer_t*> in_service() const;
};

}}}

#endif

