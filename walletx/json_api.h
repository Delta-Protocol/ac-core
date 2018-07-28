#ifndef USGOV_f195771f72f8dc2390e21a5c2a022838551620e7e997e90438f201a2aa3aa216
#define USGOV_f195771f72f8dc2390e21a5c2a022838551620e7e997e90438f201a2aa3aa216

#include <us/wallet/api.h>
#include <string>
#include <jsoncpp/json/json.h> 

namespace us { namespace wallet {

using namespace std;

struct json_api:api {
	json_api(api* underlying_api);
	virtual ~json_api();

	virtual void balance(bool detailed, ostream&os) override;
    virtual void list(bool showpriv, ostream&) override;
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
    virtual void ping(ostream&) override;

    api* underlying_api;
};

}}

#endif



