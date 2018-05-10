
#ifndef USGOV_d13c48a8154b47f3ebe88feb637366f84eff26bd36195bf8c5039244c42f9507
#define USGOV_d13c48a8154b47f3ebe88feb637366f84eff26bd36195bf8c5039244c42f9507

#include <us/gov/socket/datagram.h>
#include <us/gov/crypto.h>
#include "wallet.h"

namespace us { namespace wallet {

using namespace std;

struct api {

	virtual void balance(bool detailed, ostream&)=0;
	virtual void dump(ostream&)=0;

	void priv_key(const crypto::ec::keys::priv_t& privkey, ostream&);
	virtual void new_address(ostream&)=0;
	virtual void add_address(const crypto::ec::keys::priv_t& privkey, ostream&)=0;
	typedef wallet::tx_make_p2pkh_input tx_make_p2pkh_input;
	virtual void tx_make_p2pkh(const tx_make_p2pkh_input&, ostream&)=0;
	virtual void tx_sign(const string&txb58, cash::tx::sigcode_t sigcodei, cash::tx::sigcode_t sigcodeo, ostream&os)=0;
	virtual void tx_send(const string&txb58, ostream&os)=0;
	virtual void tx_decode(const string&txb58, ostream&os)=0;
	virtual void tx_check(const string&txb58, ostream&os)=0;

	void gen_keys(ostream&os);

};

struct rpc_api:api {
	rpc_api(const string& walletd_host, uint16_t walletd_port);
	virtual ~rpc_api();

	virtual void balance(bool detailed, ostream&os) override;
	virtual void dump(ostream&os) override;
	virtual void new_address(ostream&os) override;
	virtual void add_address(const crypto::ec::keys::priv_t& privkey, ostream&os) override;
	virtual void tx_make_p2pkh(const tx_make_p2pkh_input&, ostream&os) override;
	virtual void tx_sign(const string&txb58, cash::tx::sigcode_t sigcodei, cash::tx::sigcode_t sigcodeo, ostream&os) override;
	virtual void tx_send(const string&txb58, ostream&os) override;
	virtual void tx_decode(const string&txb58, ostream&os) override;
	virtual void tx_check(const string&txb58, ostream&os) override;


private:
	void ask(int service, ostream&os);
	void ask(int service, const string& args, ostream&os);

	string walletd_host;
	uint16_t walletd_port;

};

struct local_api:api, wallet {
	local_api(const string& homedir, const string& backend_host, uint16_t backend_port);
	virtual ~local_api();

	virtual void balance(bool detailed, ostream&os) override;
	virtual void dump(ostream&os) override;
	virtual void new_address(ostream&os) override;
	virtual void add_address(const crypto::ec::keys::priv_t& privkey, ostream&os) override;
	virtual void tx_make_p2pkh(const api::tx_make_p2pkh_input&, ostream&os) override;
	virtual void tx_sign(const string&txb58, cash::tx::sigcode_t sigcodei, cash::tx::sigcode_t sigcodeo, ostream&os) override;
	virtual void tx_send(const string&txb58, ostream&os) override;
	virtual void tx_decode(const string&txb58, ostream&os) override;
	virtual void tx_check(const string&txb58, ostream&os) override;

private:
};

}}

#endif



