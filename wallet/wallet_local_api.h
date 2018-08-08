#ifndef USGOV_b05bd77af998c823ed16a99e5cd5a0faa0ef03d000b57ee63148072bf9a65c79
#define USGOV_b05bd77af998c823ed16a99e5cd5a0faa0ef03d000b57ee63148072bf9a65c79

#include "wallet_api.h"
#include "wallet.h"

namespace us{ namespace wallet {
using namespace std;

    struct wallet_local_api: wallet_api, us::wallet::wallet {
        typedef us::wallet::wallet b;
        wallet_local_api(const string& homedir, const string& backend_host, uint16_t backend_port);
        virtual ~wallet_local_api();
        using wallet::add_address;
        using wallet_api::hash_t;
        using wallet_api::cash_t;
        using wallet_api::tx_make_p2pkh_input;

//#include <us/api/apitool_generated_wallet_functions_cpp_override>
#include <us/api/apitool_generated__functions_wallet_cpp_override>

    bool connect_backend(ostream&os);
    chrono::steady_clock::time_point connected_since;

    socket::peer_t endpoint;


    };


}}

#endif



