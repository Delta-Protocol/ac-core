#ifndef US_GOV_WALLET_PAIRING_RPC_API_H
#define US_GOV_WALLET_PAIRING_RPC_API_H

#include "pairing_api.h"
#include "rpc_api.h"

namespace us{ namespace wallet{
using namespace std;

class pairing_rpc_api: public us::wallet::pairing_api, virtual public rpc_api{
public:
    using us::wallet::pairing_api::pub_t;

    pairing_rpc_api(const rpc_api::keys&k, const string& walletd_host, uint16_t walletd_port);
    virtual ~pairing_rpc_api();

    #include <us/api/apitool_generated__functions_pairing_cpp_override> //APITOOL
};
}}

#endif



