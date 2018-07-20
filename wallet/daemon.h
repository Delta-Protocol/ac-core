#ifndef USGOV_98e8c16336dfe2bb0f4b26ffe37d808f974893b94c081f2f0f0419f11ac5e0ae
#define USGOV_98e8c16336dfe2bb0f4b26ffe37d808f974893b94c081f2f0f0419f11ac5e0ae

#include <us/gov/auth/daemon.h>
#include <us/gov/crypto/ec.h>
#include "wallet.h"
#include <unordered_map>
#include "local_api.h"

namespace us { namespace wallet {

using namespace std;

using socket::datagram;

struct peer_t;

struct wallet_daemon: auth::daemon, local_api {
	typedef auth::daemon b;
	typedef b::keys keys;
	typedef keys::pub_t pub_t;
	typedef pub_t::hash_t hash_t;

	wallet_daemon(const keys&, uint16_t port, const string& home, const string&backend_host, uint16_t backend_port);
	virtual ~wallet_daemon();
	virtual bool process_work(socket::peer_t*, datagram*) override;

   inline virtual const keys& get_keys() const override { return id; }

   virtual socket::client* create_client(int sock) override;

	const keys& id;
  //  string backend_host;
  //  uint16_t backend_port;

	bool send_response(socket::peer_t *c, datagram*d, const string& payload);
	bool send_error_response(socket::peer_t *c, datagram*d, const string& error);


    bool authorize(const pub_t& p) const;

};


}}

#endif

