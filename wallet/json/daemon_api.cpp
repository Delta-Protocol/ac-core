#include "daemon_api.h"

typedef us::wallet::json::daemon_api c;
using namespace us::wallet;
using namespace std;

c::daemon_api(us::wallet::daemon_api* underlying_api): underlying_api(underlying_api) {
}

c::~daemon_api() {
    delete underlying_api;
}

void c::balance(bool detailed0, ostream&os) {
bool detailed=true;
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
	val["addresses"]=addresses;
    }
    else {
        string p;
        is >> p;
        val["balance"]=p;
    }
    os << val;
}


void c::list(bool showpriv, ostream& os){ 
    ostringstream output;
    underlying_api->list(showpriv,output);

    Json::Value val;
    istringstream is(output.str());
    if (showpriv) {
        int n=0;
        Json::Value lists;
        while(is.good()) {
            Json::Value v;
            string s;
            is >> s;
	if(!is.good()) break;
            v["private_Key"]=s;
            is >> s;
            v["public_key"]=s;
            is >> s;
            v["address"]=s;
            lists[n++]=v;
        }
	val["keys"]=lists;
    }
    else {
        string p; 
        is >> p;
        val["private_Key"]=p;
    }
    os << val;
}


void c::new_address(ostream&os) {  
 ostringstream plain;
 underlying_api->new_address(plain);

    Json::Value val;
    istringstream is(plain.str());

    string p;
    is >> p;
    val["address"]=p;

    os << val;
}

	
void c::add_address(const gov::crypto::ec::keys::priv_t& privkey, ostream&os) {  
 ostringstream plain;
 underlying_api->add_address(privkey,plain);

    Json::Value val;
    istringstream is(plain.str());

    string p;
    is >> p;
    val["address"]=p;

    os << val;
}


void c::transfer(const gov::crypto::ripemd160::value_type&value_type, const cash_t& cash_t, ostream&os) { 


 ostringstream plain;
 underlying_api->transfer(value_type, cash_t ,plain);

    Json::Value val;
    istringstream is(plain.str());

    string p;
    is >> p;
    val["transfer"]=p;

    os << val;

}


void c::tx_make_p2pkh(const tx_make_p2pkh_input&i, ostream&os) { 

 ostringstream plain;
 underlying_api->tx_make_p2pkh(i ,plain);

    Json::Value val;
    istringstream is(plain.str());

    string p;
    is >> p;
    val["tx_make_p2pkh"]=p;

    os << val;

}


void c::tx_sign(const string&txb58, gov::cash::tx::sigcode_t sigcodei, gov::cash::tx::sigcode_t sigcodeo, ostream&os) { 

 ostringstream plain;
 underlying_api->tx_sign(txb58, sigcodei, sigcodeo ,plain);

    Json::Value val;
    istringstream is(plain.str());

    string p;
    is >> p;
    val["tx_sign"]=p;

    os << val;

}


void c::tx_send(const string&txb58, ostream&os) { 

 ostringstream plain;
 underlying_api->tx_send(txb58 ,plain);

    Json::Value val;
    istringstream is(plain.str());

    string p;
    is >> p;
    val["tx_send"]=p;

    os << val;

}

void c::tx_decode(const string&txb58, ostream&os) { 

 ostringstream plain;
 underlying_api->tx_decode(txb58 ,plain);

    Json::Value val;
    istringstream is(plain.str());

    string p;
    is >> p;
    val["tx_decode"]=p;

    os << val;
}


void c::tx_check(const string&txb58, ostream&os) { 

 ostringstream plain;
 underlying_api->tx_check(txb58 ,plain);

    Json::Value val;
    istringstream is(plain.str());

    string p;
    is >> p;
    val["tx_check"]=p;

    os << val;
}


void c::ping(ostream&os) {

 ostringstream plain;
 underlying_api->ping(plain);

    Json::Value val;
    istringstream is(plain.str());

    string p;
    is >> p;
    val["ping_response"]=p;

    os << val;
}


void c::pair(const pub_t& pk, const string& name, ostream&os) { 

 ostringstream plain;
 underlying_api->pair(pk, name , plain);

    Json::Value val;
    istringstream is(plain.str());

    string p;
    is >> p;
    val["pair"]=p;

    os << val;
}


void c::unpair(const pub_t& pk, ostream&os) { 

 ostringstream plain;
 underlying_api->unpair(pk , plain);

    Json::Value val;
    istringstream is(plain.str());

    string p;
    is >> p;
    val["unpair"]=p;

    os << val;
}


void c::list_devices(ostream&os) {	

 ostringstream plain;
 underlying_api->list_devices(plain);

    Json::Value val;
    istringstream is(plain.str());

    string p;
    is >> p;
    val["list_devices"]=p;

    os << val;
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
