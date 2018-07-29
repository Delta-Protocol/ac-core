#ifndef USGOV_539edd8ed1d3f598bd26328c77e0124d3e69f055536f18aaf86c5b2e98ee5e0e
#define USGOV_539edd8ed1d3f598bd26328c77e0124d3e69f055536f18aaf86c5b2e98ee5e0e

#include "wallet_api.h"
#include "rpc_api.h"

namespace us{ namespace wallet {
using namespace std;

    struct wallet_rpc_api: wallet_api, virtual rpc_api {
        typedef rpc_api b;
        using wallet_api::pub_t;

        wallet_rpc_api(const b::keys&k, const string& walletd_host, uint16_t walletd_port);
        virtual ~wallet_rpc_api();

//#include <us/api/apitool_generated_wallet_functions_cpp_override>
#include <us/api/apitool_generated__functions_wallet_cpp_override>


    };


}}

#endif



