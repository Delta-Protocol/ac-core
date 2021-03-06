#include "wallet_rpc_api.h"
#include "protocol.h"
#include <sstream>
#include <us/gov/socket/datagram.h>
#include <us/gov/cash.h>

typedef us::wallet::wallet_rpc_api c;
using namespace us::wallet;
using namespace std;


c::wallet_rpc_api(const b::keys&k, const string& walletd_host, uint16_t walletd_port):b(k, walletd_host, walletd_port) {
}

c::~wallet_rpc_api() {
}

void c::ask_ping(ostream&os) {
//cout << "asking " << service << " " << args << endl;
    if (!connect_walletd(os)) return;

    datagram* d=new datagram(us::wallet::protocol::wallet_ping,"");
    auto r=send_recv(d);
    if (unlikely(!r.first.empty())) {
        os << "(Error) " << r.first;
    }
    else {
        os << "Remote wallet says: " << r.second->parse_string();
        delete r.second;
    }
}

namespace {
ostream& operator << (ostream&os, bool v) {
    os << (v?1:0);
    return os;
}
};

using namespace protocol;
#include <us/api/apitool_generated__functions_wallet_cpp_rpc-impl>

/*
void c::balance(bool detailed, ostream&os) {
//for (int i=0; i<100; ++i) {
	ask(us::wallet::protocol::balance_query,detailed?"1":"0",os);
//    os << " ";
//}
}



void c::new_address(ostream&os) {
	ask(us::wallet::protocol::new_address_query,os);
}

void c::add_address(const us::gov::crypto::ec::keys::priv_t& privkey, ostream&os) {
	ostringstream k;
	k << privkey;
	ask(us::wallet::protocol::add_address_query,k.str(),os);
}


void c::tx_make_p2pkh(const tx_make_p2pkh_input&i, ostream&os) {
	ostringstream si;
	i.to_stream(si);
	ask(us::wallet::protocol::tx_make_p2pkh_query,si.str(),os);
}

void c::tx_sign(const string&txb58, us::gov::cash::tx::sigcode_t sigcodei, us::gov::cash::tx::sigcode_t sigcodeo, ostream&os) {
	ostringstream si;
	si << txb58 << ' ' << sigcodei << ' ' << sigcodeo;
	ask(us::wallet::protocol::tx_sign_query,si.str(),os);
}

void c::tx_send(const string&txb58, ostream&os) {
	ask(us::wallet::protocol::tx_send_query,txb58,os);
}

void c::tx_decode(const string&txb58, ostream&os) {
	ask(us::wallet::protocol::tx_decode_query,txb58,os);
}

void c::tx_check(const string&txb58, ostream&os) {
	ask(us::wallet::protocol::tx_check_query,txb58,os);
}

void c::ping(ostream&os) {
	ask_ping(os);
}
*/

