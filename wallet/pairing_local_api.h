#ifndef US_GOV_WALLET_PAIRING_LOCAL_API_H
#define US_GOV_WALLET_PAIRING_LOCAL_API_H

#include <us/wallet/pairing_api.h>
#include "pairing.h"

namespace us{ namespace wallet{
using namespace std;

class pairing_local_api: public us::wallet::pairing_api, public us::wallet::pairing {
public:
    using us::wallet::pairing_api::pub_t;

    pairing_local_api(const string& homedir);
    virtual ~pairing_local_api();

#include <us/api/apitool_generated__functions_pairing_cpp_override> //APITOOL
};
}}

#endif



