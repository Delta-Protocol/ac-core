
#ifndef USGOV_501a1656c74ab5296d94d111b535c5b05fb98098d2116c600b391f6e612eca22
#define USGOV_501a1656c74ab5296d94d111b535c5b05fb98098d2116c600b391f6e612eca22

#include <us/api/apitool_generated_pairing.h>
#include "pairing.h"

namespace us{ namespace wallet {
using namespace std;

    struct pairing_local_api: us::api::pairing, us::wallet::pairing {
        typedef us::wallet::pairing b;
        using us::api::pairing::pub_t;

        pairing_local_api(const string& homedir);
        virtual ~pairing_local_api();

#include <us/api/apitool_generated_pairing_functions_cpp_override>

    };


}}

#endif



