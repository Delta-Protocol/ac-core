#include "api.h"
#include "protocol.h"
#include <sstream>
#include <us/gov/socket/datagram.h>
#include <us/gov/cash.h>

using namespace us::wallet;

using datagram=socket::datagram;

void api::priv_key(const crypto::ec::keys::priv_t& privkey, ostream&os) {
	if (!crypto::ec::keys::verify(privkey)) {
		os << "The private key is incorrect." << endl;
	}
	auto pub=crypto::ec::keys::get_pubkey(privkey);
	os << "Public key: " << pub << endl;
	os << "Public key hash: " << pub.compute_hash() << endl;
}

void api::gen_keys(ostream&os) {
	crypto::ec::keys k=crypto::ec::keys::generate();
	os << "Private key: " << k.priv.to_b58() << endl;
	os << "Public key: " << k.pub.to_b58() << endl;
	os << "Address: " << k.pub.compute_hash() << endl;
}

rpc_api::rpc_api(const string& walletd_host, uint16_t walletd_port):walletd_host(walletd_host), walletd_port(walletd_port) {
}

rpc_api::~rpc_api() {
}

void rpc_api::ask(int service, ostream&os) {
	ask(service,"",os);
}

void rpc_api::ask(int service, const string& args, ostream&os) {
	datagram* q=new datagram(service,args);
	socket::datagram* response=socket::peer_t::send_recv(walletd_host, walletd_port, q);
	if (response) {
		os << response->parse_string() << endl;
		delete response;
	}
	else {
		os << "ERROR" << endl;
	}
}

void rpc_api::balance(bool detailed, ostream&os) {
	ask(us::wallet::protocol::balance_query,detailed?"1":"0",os);
}

void rpc_api::dump(ostream&os) {
	ask(us::wallet::protocol::dump_query,os);
}

void rpc_api::new_address(ostream&os) {
	ask(us::wallet::protocol::new_address_query,os);
}

void rpc_api::add_address(const crypto::ec::keys::priv_t& privkey, ostream&os) {
	ostringstream k;
	k << privkey;
	ask(us::wallet::protocol::add_address_query,k.str(),os);
}

void rpc_api::tx_make_p2pkh(const tx_make_p2pkh_input&i, ostream&os) {
	ostringstream si;
	i.to_stream(si);
	ask(us::wallet::protocol::tx_make_p2pkh_query,si.str(),os);
}

void rpc_api::tx_sign(const string&txb58, cash::tx::sigcode_t sigcodei, cash::tx::sigcode_t sigcodeo, ostream&os) {
	ostringstream si;
	si << txb58 << ' ' << sigcodei << ' ' << sigcodeo;
	ask(us::wallet::protocol::tx_sign_query,si.str(),os);
}

void rpc_api::tx_send(const string&txb58, ostream&os) {
	ask(us::wallet::protocol::tx_send_query,txb58,os);
}

void rpc_api::tx_decode(const string&txb58, ostream&os) {
	ask(us::wallet::protocol::tx_decode_query,txb58,os);
}

void rpc_api::tx_check(const string&txb58, ostream&os) {
	ask(us::wallet::protocol::tx_check_query,txb58,os);
}

void rpc_api::pair(const pub_t& pk, const string& name, ostream&os) {
	ostringstream si;
	si << pk << ' ' << name;
	ask(us::wallet::protocol::pair_query,si.str(),os);
}

void rpc_api::unpair(const pub_t& pk, ostream&os) {
	ostringstream si;
	si << pk;
	ask(us::wallet::protocol::unpair_query,si.str(),os);
}

void rpc_api::list_devices(ostream&os) {
	ask(us::wallet::protocol::list_devices_query,os);
}

//----------------local api

local_api::local_api(const string& homedir, const string& backend_host, uint16_t backend_port):wallet(homedir, backend_host, backend_port), pairing(homedir) {
}

local_api::~local_api() {
}

void local_api::balance(bool detailed, ostream&os) {
	refresh();
	if (detailed) {
		dump_balances(os);
	}
	else {
		os << wallet::balance() << endl;
	}
}

void local_api::dump(ostream&os) {
	wallet::dump(os);
}

void local_api::new_address(ostream&os) {
	os << "Address: " << wallet::new_address() << endl;
}

void local_api::add_address(const crypto::ec::keys::priv_t& privkey, ostream&os) {
	auto a=wallet::add_address(privkey);
	os << "Address: " << a << endl;
}

void local_api::tx_make_p2pkh(const api::tx_make_p2pkh_input&i, ostream&os) {
    auto tx=wallet::tx_make_p2pkh(i);
    if (tx.first.empty())
    	os << tx.second << endl;
    else 
    	os << tx.first << endl;
}

void local_api::tx_sign(const string&txb58, cash::tx::sigcode_t sigcodei, cash::tx::sigcode_t sigcodeo, ostream&os) {
    auto tx=wallet::tx_sign(txb58,sigcodei,sigcodeo);
	if (tx.first.empty())
	    os << tx.second << endl;
	else
	    os << tx.first << endl;
}

void local_api::tx_send(const string&txb58, ostream&os) {
	wallet::send(cash::tx::from_b58(txb58));
	os << "sent" << endl;
}
   
void local_api::tx_decode(const string&txb58, ostream&os) {
	cash::tx t=cash::tx::from_b58(txb58);
	t.write_pretty(os);
}
     
void local_api::tx_check(const string&txb58, ostream&os) {
	cash::tx t=cash::tx::from_b58(txb58);
	auto fee=t.check();
	if (fee<=0) {
		os << "Individual inputs and fees must be positive." << endl;
	}
	os << "Looks ok." << endl;
}

void local_api::pair(const pub_t& pk, const string& name, ostream&os) {
    devices.pair(pk,name);    
	os << "done." << endl;
}

void local_api::unpair(const pub_t& pk, ostream&os) {
    devices.unpair(pk);    
	os << "done." << endl;
}

void local_api::list_devices(ostream&os) {
    devices.dump(os);    
}

