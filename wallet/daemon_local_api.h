#ifndef USGOV_ca166ff10aed048381709f1009faa3b5040c245209c75c126fd4984045d546a2
#define USGOV_ca166ff10aed048381709f1009faa3b5040c245209c75c126fd4984045d546a2

#include <us/api/apitool_generated_wallet.h>
#include "wallet_local_api.h"
#include "pairing_local_api.h"

namespace us{ namespace wallet {
using namespace std;

    struct daemon_local_api: us::api::wallet_daemon, wallet_local_api, pairing_local_api {
        typedef wallet_local_api w;
        typedef pairing_local_api p;
        daemon_local_api(const string& home, const string& backend_host, uint16_t backend_port): w(home,backend_host,backend_port), p(home) {}
        virtual ~daemon_local_api() {}
        using w::pub_t;

#include <us/api/apitool_generated_wallet_daemon_functions_cpp_impl>

    };


}}

#endif



