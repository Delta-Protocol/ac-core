
#ifndef USGOV_be7dc84108784c4af35dada40b3f9b325e2d9f0782781c75529a2c286e0cfa5c
#define USGOV_be7dc84108784c4af35dada40b3f9b325e2d9f0782781c75529a2c286e0cfa5c

#include "api.h"
#include "peer_t.h"
#include <chrono>


namespace us { namespace wallet {

using namespace std;

struct rpc_api:api, peer_t {
	typedef peer_t b;
	using b::pub_t;

	rpc_api(const b::keys&, const string& walletd_host, uint16_t walletd_port);
	virtual ~rpc_api();

	virtual void balance(bool detailed, ostream&os) override;
	virtual void dump(ostream&os) override;
	virtual void new_address(ostream&os) override;
	virtual void add_address(const priv_t&, ostream&os) override;
	virtual void tx_make_p2pkh(const tx_make_p2pkh_input&, ostream&os) override;
	virtual void tx_sign(const string&txb58, sigcode_t inputs, sigcode_t outputs, ostream&os) override;
	virtual void tx_send(const string&txb58, ostream&os) override;
	virtual void tx_decode(const string&txb58, ostream&os) override;
	virtual void tx_check(const string&txb58, ostream&os) override;
	virtual void pair(const pub_t&, const string& name, ostream&os) override;
	virtual void unpair(const pub_t&, ostream&os) override;
	virtual void list_devices(ostream&os) override;
    virtual void ping_gov(ostream&) override;
    virtual void ping_wallet(ostream&) override;

private:
	void ask(int service, ostream&os);
	void ask(int service, const string& args, ostream&os);

	string walletd_host;
	uint16_t walletd_port;

    bool connect_walletd(ostream&);
    chrono::steady_clock::time_point connected_since;
    

};

}}

#endif



