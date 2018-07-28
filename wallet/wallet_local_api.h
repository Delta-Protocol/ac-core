#ifndef USGOV_b05bd77af998c823ed16a99e5cd5a0faa0ef03d000b57ee63148072bf9a65c79
#define USGOV_b05bd77af998c823ed16a99e5cd5a0faa0ef03d000b57ee63148072bf9a65c79

#include <us/api/apitool_generated_wallet.h>
#include "wallet.h"

namespace us{ namespace wallet {
using namespace std;

    struct wallet_local_api: us::api::wallet, us::wallet::wallet {
        typedef us::wallet::wallet b;
        wallet_local_api(const string& homedir, const string& backend_host, uint16_t backend_port);
        virtual ~wallet_local_api();
        using wallet::add_address;

#include <us/api/apitool_generated_wallet_functions_cpp_override>

    bool connect_backend(ostream&os);
    chrono::steady_clock::time_point connected_since;

    socket::peer_t endpoint;


    };


}}

#endif



