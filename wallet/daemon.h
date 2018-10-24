#ifndef US_GOV_WALLET_DAEMON_H
#define US_GOV_WALLET_DAEMON_H

#include <unordered_map>
#include "daemon_local_api.h"
#include <us/gov/auth/daemon.h>
#include <us/gov/crypto/ec.h>
#include "wallet.h"

namespace us{ namespace wallet{

using namespace std;
using socket::datagram;

class peer_t;

class wallet_daemon: public auth::daemon, public daemon_local_api {
public:
    typedef auth::daemon base_1;
    typedef base_1::keys keys;
    typedef keys::pub_t pub_t;
    typedef pub_t::hash_t hash_t;

    wallet_daemon(const keys&, uint16_t port, const string& home, 
                  const string&backend_host, uint16_t backend_port);
    virtual ~wallet_daemon();
    virtual bool process_work(socket::peer_t*, datagram*) override;

    using wallet::add_address;
    #include <us/api/apitool_generated__protocol_wallet-daemon_cpp_service_handler_headers>

    inline virtual const keys& get_keys() const override { 
        return m_id; 
    }

    virtual socket::client* create_client(int sock) override;


    bool send_response(socket::peer_t *c, datagram*d, const string& payload);
    bool send_error_response(socket::peer_t *c, datagram*d, const string& error);

    bool authorize(const pub_t& p) const;

private:
    const keys& m_id;
};

}}

#endif

