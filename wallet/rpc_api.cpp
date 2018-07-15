#include "rpc_api.h"
#include "protocol.h"
#include <sstream>
#include <us/gov/socket/datagram.h>
#include <us/gov/cash.h>

typedef us::wallet::rpc_api c;
using namespace us::wallet;
using namespace std;

using datagram=socket::datagram;

c::rpc_api(const b::keys&k, const string& walletd_host, uint16_t walletd_port):walletd_host(walletd_host), walletd_port(walletd_port), b(k,0) {
}

c::~rpc_api() {
}

void c::ask(int service, ostream&os) {
	ask(service,"",os);
}

void c::ask(int service, const string& args, ostream&os) {
	if (sock==0) {
//		cout << "Iniciando auth" << endl;
		if (!connect(walletd_host,walletd_port,true)) {
			os << "Error. Unable to connect to wallet daemon.";
			return;
		}
        auto r=run_auth();
        if (unlikely(!r.empty())) {
            os << r;
            return;
        }
	}
	datagram* d=new datagram(service,args);

    auto r=send_recv(d);

//    send(d);
//    auto r=recv_response(); //complete_datagram(3);

        

//        auto response=new datagram();
/*
        while (!response->completed()) {
                if (!response->complete_datagram(sock,2)) {
                        delete response;
                        os << "Error. Connected but unable to receive a response.";
			return;
                }
        }
*/
	//socket::datagram* response=socket::peer_t::send_recv(walletd_host, walletd_port, d);
    if (unlikely(!r.first.empty())) {
//cout << "ZZCCZZZZZZ" << endl;
//r.second->dump(cout);
        os << r.first;
    }
    else {
//cout << "ZZZZZZZZ" << endl;
//r.second->dump(cout);
        //cout << "datagram" << response->size() << endl;
    	os << r.second->parse_string();
	    delete r.second;
    }
}

void c::balance(bool detailed, ostream&os) {
	ask(us::wallet::protocol::balance_query,detailed?"1":"0",os);
}

void c::dump(ostream&os) {
	ask(us::wallet::protocol::dump_query,os);
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

