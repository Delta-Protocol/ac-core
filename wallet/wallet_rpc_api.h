#ifndef US_GOV_WALLET_WALLET_RPC_API_H
#define US_GOV_WALLET_WALLET_RPC_API_H

#include "wallet_api.h"
#include "rpc_api.h"

namespace us{ namespace wallet{
using namespace std;

class wallet_rpc_api: public wallet_api, virtual public rpc_api {
public:
    using wallet_api::pub_t;

    wallet_rpc_api(const rpc_api::keys&k, const string& walletd_host, uint16_t walletd_port);
    virtual ~wallet_rpc_api();

    void ask_ping(ostream&os);

#include <us/api/apitool_generated__functions_wallet_cpp_override> //APITOOL
};
}}

#endif



