#include "daemon_api.h"

typedef us::wallet::json::daemon_api c;
using namespace us::wallet;
using namespace std;

c::daemon_api(us::wallet::daemon_api* underlying_api): underlying_api(underlying_api) {
}

c::~daemon_api() {
    delete underlying_api;
}

void c::balance(bool detailed, ostream&os) {
    ostringstream plain;
    underlying_api->balance(detailed,plain);

    Json::Value val;
    istringstream is(plain.str());
    if (detailed) {
        int n=0;
        Json::Value addresses;
        while(is.good()) {
            Json::Value v;
            string s;
            is >> s;
            v["address"]=s;
            is >> s;
            v["locking_program"]=s;
            is >> s;
            v["balance"]=s;
            addresses[n++]=v;
        }
        val["addresses"]=val;
    }
    else {
        string p;
        is >> p;
        val["balance"]=p;
    }
    os << val;
}

void c::list(bool showpriv, ostream& os) {
    os << "Not implemented" << endl;
}

void c::new_address(ostream&os) {
    os << "Not implemented" << endl;
}

void c::add_address(const gov::crypto::ec::keys::priv_t& privkey, ostream&os) {
	ostringstream k;
	k << privkey;
    os << "Not implemented" << endl;
}

void c::transfer(const gov::crypto::ripemd160::value_type&, const cash_t&, ostream&os) {
    os << "Not implemented" << endl;
}

void c::tx_make_p2pkh(const tx_make_p2pkh_input&i, ostream&os) {
	ostringstream si;
	i.to_stream(si);
    os << "Not implemented" << endl;
}

void c::tx_sign(const string&txb58, gov::cash::tx::sigcode_t sigcodei, gov::cash::tx::sigcode_t sigcodeo, ostream&os) {
	ostringstream si;
	si << txb58 << ' ' << sigcodei << ' ' << sigcodeo;
    os << "Not implemented" << endl;
}

void c::tx_send(const string&txb58, ostream&os) {
    os << "Not implemented" << endl;
}

void c::tx_decode(const string&txb58, ostream&os) {
    os << "Not implemented" << endl;
}

void c::tx_check(const string&txb58, ostream&os) {
    os << "Not implemented" << endl;
}

void c::ping(ostream&os) {
    os << "Not implemented" << endl;
}

void c::pair(const pub_t& pk, const string& name, ostream&os) {
	ostringstream si;
	si << pk << ' ' << name;
    os << "Not implemented" << endl;
}

void c::unpair(const pub_t& pk, ostream&os) {
	ostringstream si;
	si << pk;
    os << "Not implemented" << endl;
}

void c::list_devices(ostream&os) {
    os << "Not implemented" << endl;
}

/*
#ifdef FCGI
#include "json.h"
#include <sstream>

using namespace std;
using namespace us::wallet::w3api;
typedef us::wallet::w3api::json c;

Json::Value c::convert_response_balance(const string& s) {
}
Json::Value c::convert_response_balance_detailed(const string& s) {
    istringstream is(s);
    string p;
    is >> p;
    Json::Value val;
    val["dbalance"]=p;
    return val;
}
*/
