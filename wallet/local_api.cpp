#include "local_api.h"
#include "protocol.h"
#include <sstream>
#include <us/gov/socket/datagram.h>
#include <us/gov/cash.h>

typedef us::wallet::local_api c;
using namespace us::wallet;
using namespace std;

using datagram=socket::datagram;

c::local_api(const string& homedir, const string& backend_host, uint16_t backend_port):wallet(homedir, backend_host, backend_port), pairing(homedir) {
}

c::~local_api() {
}

bool c::bring_up_backend(ostream&os) {
    if (endpoint.connected()) return true;
cout << "---connecting" << endl;
    if (!endpoint.connect(backend_host,backend_port,true)) {
        cerr << "wallet: unable to connect to " << backend_host << ":" << backend_port << endl;
        os << "Error. Backend is unreachable.";
        return false;
    }
    return true;
}

void c::balance(bool detailed, ostream&os) {
    if (!bring_up_backend(os)) return;
cout << "---connected" << endl;
	refresh(endpoint);
	if (detailed) {
		extended_balance(os);
	}
	else {
		os << wallet::balance();
	}
}

void c::dump(ostream&os) {
	wallet::dump(os);
}

void c::new_address(ostream&os) {
	os << wallet::new_address() << endl;
}

void c::add_address(const crypto::ec::keys::priv_t& privkey, ostream&os) {
	auto a=wallet::add_address(privkey);
	os << a << endl;
}

void c::tx_make_p2pkh(const api::tx_make_p2pkh_input&i, ostream& os) {
    if (!bring_up_backend(os)) return;

    auto tx=wallet::tx_make_p2pkh(endpoint,i);
    if (tx.first.empty())
    	os << tx.second << endl;
    else 
    	os << tx.first << endl;
}

void c::tx_sign(const string&txb58, cash::tx::sigcode_t sigcodei, cash::tx::sigcode_t sigcodeo, ostream&os) {
    if (!bring_up_backend(os)) return;

    auto tx=wallet::tx_sign(endpoint, txb58, sigcodei, sigcodeo);
	if (tx.first.empty())
	    os << tx.second << endl;
	else
	    os << tx.first << endl;
}

void c::tx_send(const string&txb58, ostream&os) {
    if (!bring_up_backend(os)) return;

	string e=wallet::send(endpoint, cash::tx::from_b58(txb58));
    if (e.empty()) {
	    os << "Successfully sent :)" << endl;
    }
    else {
	    os << e << endl;
    }
}

void c::tx_decode(const string&txb58, ostream&os) {
	cash::tx t=cash::tx::from_b58(txb58);
	t.write_pretty(os);
}

void c::tx_check(const string&txb58, ostream&os) {
	cash::tx t=cash::tx::from_b58(txb58);
	auto fee=t.check();
	if (fee<=0) {
		os << "Individual inputs and fees must be positive." << endl;
	}
	os << "Looks ok." << endl;
}

void c::pair(const pub_t& pk, const string& name, ostream&os) {
    devices.pair(pk,name);
	os << "done." << endl;
}

void c::unpair(const pub_t& pk, ostream&os) {
    devices.unpair(pk);
	os << "done." << endl;
}

void c::list_devices(ostream&os) {
    devices.dump(os);
}
