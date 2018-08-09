
#ifndef USGOV_be7dc84108784c4af35dada40b3f9b325e2d9f0782781c75529a2c286e0cfa5c
#define USGOV_be7dc84108784c4af35dada40b3f9b325e2d9f0782781c75529a2c286e0cfa5c

//#include <us/api/wallet.h>
#include <us/gov/auth/peer_t.h>
#include <chrono>


namespace us { namespace wallet {

using namespace std;

struct rpc_api: gov::auth::peer_t {
	typedef peer_t b;
	using b::pub_t;

	rpc_api(const b::keys&, const string& walletd_host, uint16_t walletd_port);
	virtual ~rpc_api();

    inline virtual const keys& get_keys() const override { return id; }

    virtual bool authorize(const pubkey_t& p) const override { return true; }

	void ask(int service, ostream&os);
	void ask(int service, const string& args, ostream&os);

	string walletd_host;
	uint16_t walletd_port;

	keys id;
    bool connect_walletd(ostream&);
    chrono::steady_clock::time_point connected_since;
    

};

}}

#endif



