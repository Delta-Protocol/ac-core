#ifndef USGOV_cb0510d36e522cedfae7718c0a9b9ae0db174fcf4eea11e0b8f937e23c1dcab0
#define USGOV_cb0510d36e522cedfae7718c0a9b9ae0db174fcf4eea11e0b8f937e23c1dcab0

#include "api.h"
#include "wallet.h"
#include "pairing_local_api.h"
#include "wallet_local_api.h"
#include <us/gov/socket.h>
#include <chrono>

namespace us { namespace wallet {
using namespace std;

struct local_api: pairing_local_api, wallet_local_api {
	using wallet_local_api::pub_t;

	local_api(const string& homedir, const string& backend_host, uint16_t backend_port);
	virtual ~local_api();

//    virtual void ping_gov(ostream&) override;
//    virtual void ping_wallet(ostream&) override;
};

}}

#endif



