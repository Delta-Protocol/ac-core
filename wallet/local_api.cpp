#include "local_api.h"
#include "protocol.h"
#include <sstream>
#include <us/gov/socket/datagram.h>
#include <us/gov/cash.h>

typedef us::wallet::local_api c;
using namespace us::wallet;
using namespace std;

using datagram=socket::datagram;

c::local_api(const string& homedir, const string& backend_host, uint16_t backend_port): wallet(homedir, backend_host, backend_port), pairing(homedir) {
}

c::~local_api() {
}

bool c::connect_backend(ostream&os) {
    if (endpoint.connected()) return true;
//cout << "---connecting" << endl;
    auto r=endpoint.connect(backend_host,backend_port,true);
    if (!r.empty()) {
//        cerr << r << endl; //"wallet: unable to connect to " << backend_host << ":" << backend_port << endl;
        os << "Error. " << r;
        return false;
    }
    connected_since=chrono::steady_clock::now();
    return true;
}

void c::balance(bool detailed, ostream&os) {
    if (!connect_backend(os)) return;
//cout << "---connected" << endl;
	auto r=refresh(endpoint);
    if (unlikely(!r.empty())) {
        os << r;
        return;
    }
	if (detailed) {
	    extended_balance(os);
	}
	else {
//cout << "Writting " << wallet::balance() << endl;
	    os << wallet::balance();
	}
}

void c::list(bool showpriv, ostream& os) {     
	wallet::list(showpriv,os);
}

void c::new_address(ostream&os) {
	os << wallet::new_address();
}

void c::add_address(const crypto::ec::keys::priv_t& privkey, ostream&os) {
	os << wallet::add_address(privkey);
}

void c::tx_make_p2pkh(const api::tx_make_p2pkh_input&i, ostream& os) {
    if (!connect_backend(os)) return;

    auto tx=wallet::tx_make_p2pkh(endpoint,i);
    if (tx.first.empty())
    	os << tx.second;
    else 
    	os << tx.first;
}

void c::tx_sign(const string&txb58, cash::tx::sigcode_t sigcodei, cash::tx::sigcode_t sigcodeo, ostream&os) {
    if (!connect_backend(os)) return;

    auto tx=wallet::tx_sign(endpoint, txb58, sigcodei, sigcodeo);
	if (tx.first.empty())
	    os << tx.second;
	else
	    os << tx.first;
}

void c::tx_send(const string&txb58, ostream&os) {
    if (!connect_backend(os)) return;
    auto r=cash::tx::from_b58(txb58);
    if (unlikely(!r.first.empty())) {
        os << r.first;
        return;
    }
	string e=wallet::send(endpoint, r.second);
    if (unlikely(!e.empty())) {
	    os << e;
        return;
    }
    os << "Successfully sent :)";
}

void c::tx_decode(const string&txb58, ostream&os) {
	auto r=cash::tx::from_b58(txb58);
    if (unlikely(!r.first.empty())) {
        os << r.first;
        return;
    }
	r.second.write_pretty(os);
}

void c::tx_check(const string&txb58, ostream&os) {
	auto r=cash::tx::from_b58(txb58);
    if (unlikely(!r.first.empty())) {
        os << "Cannot decode base58";
        return;
    }
    const cash::tx& t=r.second;
    bool fail=false;
	auto fee=t.check();
	if (fee<=0) {
		os << "Individual inputs and fees must be positive.";
        fail=true;
	}
	if (false) {
        if (fail) os << endl;
		os << "All in all you're just a nother brick in the wall.";
        fail=true;
	}
    if (fail) {
	    os << "This transaction looks ok.";
    }
}

void c::pair(const pub_t& pk, const string& name, ostream&os) {
    if (!pk.valid) {
        os << "Error: invalid public key.";
        return;
    }
    devices.pair(pk,name);
	os << "done.";
}

void c::unpair(const pub_t& pk, ostream&os) {
    devices.unpair(pk);
	os << "done.";
}

void c::list_devices(ostream&os) {
    devices.dump(os);
}

#include <us/gov/socket/datagram.h>
#include <utility>
#include <string>
using us::gov::socket::datagram;
/*
void c::ping_gov(ostream& os) {

    if (!connect_backend(os)) return;


    auto r=endpoint.send_recv(new datagram(us::gov::protocol::ping,""));
    if (!r.first.empty()) {
        os <<r.first;
        return;
    }
    os << r.second->parse_string();
    delete r.second;
}
*/
void c::ping(ostream& os) {
    os << "I am responsive. The backend... ";
    if (!connect_backend(os)) {
        return;
    }
    auto r=endpoint.send_recv(new datagram(us::gov::protocol::ping,""),us::gov::protocol::pong);
    if (!r.first.empty()) {
        assert(r.second==0);
        os << "is not responding (Error): " << r.first;
        return;
    }
    datagram* d=r.second;
    os << "is also responsive, it said: " << d->parse_string();
    if (!r.first.empty()) {
        assert(r.second==0);
        os << "is not responding (Error): " << r.first;
        return;
    }
    delete d;
}


