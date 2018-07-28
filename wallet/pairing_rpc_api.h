#ifndef USGOV_3e66a1481911a1a43b6b1ea9e52189d8bb219d23e19636462d0d8d34f7b5671a
#define USGOV_3e66a1481911a1a43b6b1ea9e52189d8bb219d23e19636462d0d8d34f7b5671a

#include "pairing_api.h"
#include "rpc_api.h"

namespace us{ namespace wallet {
using namespace std;

    struct pairing_rpc_api: us::wallet::pairing_api, virtual rpc_api {
        typedef rpc_api b;
        using us::wallet::pairing_api::pub_t;

        pairing_rpc_api(const b::keys&k, const string& walletd_host, uint16_t walletd_port);
        virtual ~pairing_rpc_api();

#include <us/api/apitool_generated_pairing_functions_cpp_override>

    };


}}

#endif



