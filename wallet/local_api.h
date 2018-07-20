#ifndef USGOV_cb0510d36e522cedfae7718c0a9b9ae0db174fcf4eea11e0b8f937e23c1dcab0
#define USGOV_cb0510d36e522cedfae7718c0a9b9ae0db174fcf4eea11e0b8f937e23c1dcab0

#include "api.h"
#include "wallet.h"
#include "pairing.h"
#include <us/gov/socket.h>
#include <chrono>

namespace us { namespace wallet {
using namespace std;

struct local_api:api, wallet, pairing {
	using api::pub_t;

	local_api(const string& homedir, const string& backend_host, uint16_t backend_port);
	virtual ~local_api();

	virtual void balance(bool detailed, ostream&os) override;
    virtual void list(bool showpriv, ostream&) override;

	virtual void new_address(ostream&os) override;
    using wallet::add_address;
	virtual void add_address(const priv_t& privkey, ostream&os) override;
	virtual void tx_make_p2pkh(const api::tx_make_p2pkh_input&, ostream&os) override;
	virtual void tx_sign(const string&txb58, sigcode_t inputs, sigcode_t outputs, ostream&os) override;
	virtual void tx_send(const string&txb58, ostream&os) override;
	virtual void tx_decode(const string&txb58, ostream&os) override;
	virtual void tx_check(const string&txb58, ostream&os) override;
	virtual void pair(const pub_t&, const string& name, ostream&os) override;
	virtual void unpair(const pub_t&, ostream&os) override;
	virtual void list_devices(ostream&os) override;
    virtual void ping(ostream&) override;
//    virtual void ping_gov(ostream&) override;
//    virtual void ping_wallet(ostream&) override;

    bool connect_backend(ostream&os);
    chrono::steady_clock::time_point connected_since;

    socket::peer_t endpoint;
};

}}

#endif



