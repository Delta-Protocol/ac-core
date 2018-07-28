#ifndef USGOV_ca166ff10aed048381709f1009faa3b5040c245209c75c126fd4984045d546a2
#define USGOV_ca166ff10aed048381709f1009faa3b5040c245209c75c126fd4984045d546a2

#include "daemon_api.h"
#include "wallet_local_api.h"
#include "pairing_local_api.h"

namespace us{ namespace wallet {
using namespace std;

    struct daemon_local_api: daemon_api, wallet_local_api, pairing_local_api {
        typedef wallet_local_api w;
        typedef pairing_local_api p;
        daemon_local_api(const string& home, const string& backend_host, uint16_t backend_port): w(home,backend_host,backend_port), p(home) {}
        virtual ~daemon_local_api() {}
        using w::pub_t;
        using w::hash_t;
        using w::cash_t;
        using w::tx_make_p2pkh_input;

#include <us/api/apitool_generated_wallet_daemon_functions_cpp_impl>

    };


}}

#endif


