#ifndef USGOV_46f2187412c13fb1cda264a13918b064da18651df7b676f60bd3b649a5c59f42
#define USGOV_46f2187412c13fb1cda264a13918b064da18651df7b676f60bd3b649a5c59f42

#include <us/api/apitool_generated_wallet.h>
#include "wallet_rpc_api.h"
#include "pairing_rpc_api.h"

namespace us{ namespace wallet {
using namespace std;

    struct daemon_rpc_api: us::api::wallet_daemon, virtual wallet_rpc_api, virtual pairing_rpc_api {
        typedef wallet_rpc_api w;
        typedef pairing_rpc_api p;
        daemon_rpc_api(const w::keys&k, const string& whost, uint16_t wport): w(k, whost, wport), p(k, whost, wport), rpc_api(k, whost, wport) {}
        virtual ~daemon_rpc_api() {}
        using w::pub_t;

#include <us/api/apitool_generated_wallet_daemon_functions_cpp_impl>

    };


}}

#endif



