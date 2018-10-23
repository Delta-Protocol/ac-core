#ifndef US_GOV_WALLET_RPC_API_H
#define US_GOV_WALLET_RPC_API_H

#include <chrono>
#include <us/gov/auth/peer_t.h>

namespace us{ namespace wallet{

using namespace std;

class rpc_api: public gov::auth::peer_t{
public:

    rpc_api(const gov::auth::peer_t::keys&, const string& walletd_host, uint16_t walletd_port);
    virtual ~rpc_api();

    inline virtual const keys& get_keys() const override { 
        return m_id; 
    }

    virtual bool authorize(const pubkey_t& p) const override { 
        return true; 
    }

    void ask(uint16_t service, ostream&os);
    void ask(uint16_t service, const string& args, ostream&os);

    bool connect_walletd(ostream&);

private:
    string m_walletd_host;
    uint16_t m_walletd_port;
    keys m_id;
    chrono::steady_clock::time_point m_connected_since;
};
}}

#endif



