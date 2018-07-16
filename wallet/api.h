
#ifndef USGOV_d13c48a8154b47f3ebe88feb637366f84eff26bd36195bf8c5039244c42f9507
#define USGOV_d13c48a8154b47f3ebe88feb637366f84eff26bd36195bf8c5039244c42f9507

#include <us/gov/socket/datagram.h>
#include <us/gov/crypto.h>
#include <us/gov/auth.h>
#include "wallet.h"

namespace us { namespace wallet {

using namespace std;

struct api {
    typedef gov::crypto::ec::keys::priv_t priv_t;
    typedef gov::crypto::ec::keys::pub_t pub_t;
    typedef cash::tx::sigcode_t sigcode_t;

    static void priv_key(const priv_t& privkey, ostream&);
    virtual ~api() { }

	virtual void balance(bool detailed, ostream&)=0;
	virtual void dump(ostream&)=0;

	virtual void new_address(ostream&)=0;
	virtual void add_address(const priv_t&, ostream&)=0;
	typedef wallet::tx_make_p2pkh_input tx_make_p2pkh_input;
	virtual void tx_make_p2pkh(const tx_make_p2pkh_input&, ostream&)=0;
	virtual void tx_sign(const string&txb58, sigcode_t inputs, sigcode_t outputs, ostream&os)=0;
	virtual void tx_send(const string&txb58, ostream&os)=0;
	virtual void tx_decode(const string&txb58, ostream&os)=0;
	virtual void tx_check(const string&txb58, ostream&os)=0;
	virtual void pair(const pub_t&, const string& name, ostream&os)=0;
	virtual void unpair(const pub_t&, ostream&os)=0;
	virtual void list_devices(ostream&os)=0;
	virtual void ping_gov(ostream&os)=0;
	virtual void ping_wallet(ostream&os)=0;

	void gen_keys(ostream&os);
};

}}

#endif



