#include "rpc_api.h"
#include "protocol.h"
#include <sstream>
#include <us/gov/socket/datagram.h>
#include <us/gov/cash.h>

typedef us::wallet::rpc_api c;
using namespace us::wallet;
using namespace std;


using datagram=socket::datagram;

c::rpc_api(const b::keys&k, const string& walletd_host, uint16_t walletd_port):walletd_host(walletd_host), walletd_port(walletd_port), b(0), id(k) {
}

c::~rpc_api() {
}

bool c::connect_walletd(ostream&os) {
    if (socket::peer_t::connected()) return true;
//cout << "---connecting" << endl;
    auto r=connect(walletd_host,walletd_port,true);
    if (unlikely(!r.empty())) { //connect(walletd_host,walletd_port,true)) {
//        cerr << r << endl; //"wallet: unable to connect to " << walletd_host << ":" << walletd_port << endl;
		os << "Error. " << r;
        return false;
    }
//cout << "---running auth" << endl;
    r=run_auth();
//cout << "---run auth " << id::peer_t::stagestr[stage_me] << " " << id::peer_t::stagestr[stage_peer] << endl;
    if (unlikely(!r.empty())) {
         os << r;
         return false;
    }
    if (!socket::peer_t::connected()) {
         os << "Auth failed";
         return false;
    }
    connected_since=chrono::steady_clock::now();
//cout << "---connected" << endl;
    return true;
}

void c::ask(int service, ostream&os) {
	ask(service,"",os);
}

//chrono::steady_clock::time_point

void c::ask(int service, const string& args, ostream&os) {
//cout << "asking " << service << " " << args << endl;
	if (!connect_walletd(os)) return;

	datagram* d=new datagram(service,args);
    auto r=send_recv(d);
    if (unlikely(!r.first.empty())) {
        os << r.first;
    }
    else {
    	os << r.second->parse_string();
	    delete r.second;
    }
}

void c::ask_ping(ostream&os) {
//cout << "asking " << service << " " << args << endl;
	if (!connect_walletd(os)) return;

	datagram* d=new datagram(us::wallet::protocol::ping,"");
    auto r=send_recv(d);
    if (unlikely(!r.first.empty())) {
        os << "(Error) " << r.first;
    }
    else {
    	os << "Remote wallet says: " << r.second->parse_string();
	    delete r.second;
    }
}

void c::balance(bool detailed, ostream&os) {
//for (int i=0; i<100; ++i) {
	ask(us::wallet::protocol::balance_query,detailed?"1":"0",os);
//    os << " ";
//}
}

void c::list(bool showpriv, ostream& os) {
	ask(us::wallet::protocol::list_query,showpriv?"1":"0",os);
}

void c::new_address(ostream&os) {
	ask(us::wallet::protocol::new_address_query,os);
}

void c::add_address(const crypto::ec::keys::priv_t& privkey, ostream&os) {
	ostringstream k;
	k << privkey;
	ask(us::wallet::protocol::add_address_query,k.str(),os);
}

void c::tx_make_p2pkh(const tx_make_p2pkh_input&i, ostream&os) {
	ostringstream si;
	i.to_stream(si);
	ask(us::wallet::protocol::tx_make_p2pkh_query,si.str(),os);
}

void c::tx_sign(const string&txb58, cash::tx::sigcode_t sigcodei, cash::tx::sigcode_t sigcodeo, ostream&os) {
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

void c::pair(const pub_t& pk, const string& name, ostream&os) {
	ostringstream si;
	si << pk << ' ' << name;
	ask(us::wallet::protocol::pair_query,si.str(),os);
}

void c::unpair(const pub_t& pk, ostream&os) {
	ostringstream si;
	si << pk;
	ask(us::wallet::protocol::unpair_query,si.str(),os);
}

void c::list_devices(ostream&os) {
	ask(us::wallet::protocol::list_devices_query,os);
}

void c::ping(ostream&os) {
	ask_ping(os);
}
/*
void c::ping_wallet(ostream&os) {
	ask(us::wallet::protocol::ping,os);
}
*/
