
#ifndef USGOV_d13c48a8154b47f3ebe88feb637366f84eff26bd36195bf8c5039244c42f9507
#define USGOV_d13c48a8154b47f3ebe88feb637366f84eff26bd36195bf8c5039244c42f9507

#include "wallet.h"
#include "pairing.h"

namespace us { namespace wallet {

using namespace std;

struct api: wallet::api, pairing::api {

    virtual ~api() { }

    


};

}}

#endif



