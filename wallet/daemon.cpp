
#include "daemon.h"
#include "protocol.h"
#include "peer_t.h"

using namespace us::wallet;
using namespace std;
typedef us::wallet::wallet_daemon c;

c::wallet_daemon(const keys& k, uint16_t port, const string& home, const string&backend_host, uint16_t backend_port): b(port,2), wallet_local_api(home,backend_host,backend_port), pairing_local_api(home), id(k) {
    assert(!home.empty());
}

c::~wallet_daemon() {
}

bool c::send_error_response(socket::peer_t *c, datagram*d, const string& error) {
    auto s=d->service;
	delete d;
	c->send(new datagram(s+1,"E "+error)); //+1 means answer
	return true;
}

bool c::send_response(socket::peer_t *c, datagram*d, const string& payload) {
    auto s=d->service;
	delete d;
	c->send(new datagram(s+1,payload));
	return true;
}




// wallet - master file: us/apitool/data/wallet
bool c::send_response__wallet_balance(socket::peer_t* c, socket::datagram* d) {
            istringstream is(d->parse_string());
            bool detailed; //=d->parse_string()=="1";
            is >> detailed;
            ostringstream ans;
            wallet_local_api::balance(detailed,ans);
            return send_response(c,d,ans.str());
}

bool c::send_response__wallet_list(socket::peer_t* c, socket::datagram* d) {
            istringstream is(d->parse_string());
            bool showpriv; //=d->parse_string()=="1";
            is >> showpriv;
//           bool showpriv=d->parse_string()=="1";
            ostringstream ans;
            wallet_local_api::list(showpriv,ans);
            return send_response(c,d,ans.str());
}

bool c::send_response__wallet_new_address(socket::peer_t* c, socket::datagram* d) {
           ostringstream ans;
            wallet_local_api::new_address(ans);
            return send_response(c,d,ans.str());
}

bool c::send_response__wallet_add_address(socket::peer_t* c, socket::datagram* d) {
			crypto::ec::keys::priv_t privkey;
			istringstream is(d->parse_string());
			is >> privkey;
			ostringstream ans;
			wallet_local_api::add_address(privkey,ans);
			return send_response(c,d,ans.str());
}

bool c::send_response__wallet_transfer(socket::peer_t* c, socket::datagram* d) {
			hash_t addr;
			cash_t amount;
			istringstream is(d->parse_string());
			is >> addr;
			is >> amount;
			ostringstream ans;
			wallet_local_api::transfer(addr,amount,ans);
			return send_response(c,d,ans.str());
}

bool c::send_response__wallet_tx_make_p2pkh(socket::peer_t* c, socket::datagram* d) {
			istringstream is(d->parse_string());
			wallet::tx_make_p2pkh_input i=wallet::tx_make_p2pkh_input::from_stream(is);
            if (unlikely(is.fail())) {
                return send_error_response(c,d,"Unacceptable input.");
            }

			ostringstream ans;
			wallet_local_api::tx_make_p2pkh(i,ans);
			return send_response(c,d,ans.str());
}

bool c::send_response__wallet_tx_sign(socket::peer_t* c, socket::datagram* d) {
	        string txb58;
            cash::tx::sigcode_t sci;
            cash::tx::sigcode_t sco;
			istringstream is(d->parse_string());
            is >> txb58;
            is >> sci;
            is >> sco;
			ostringstream ans;
			wallet_local_api::tx_sign(txb58,sci,sco,ans);
			return send_response(c,d,ans.str());
}

bool c::send_response__wallet_tx_send(socket::peer_t* c, socket::datagram* d) {
	        string txb58;
			istringstream is(d->parse_string());
            is >> txb58;
			ostringstream ans;
			wallet_local_api::tx_send(txb58,ans);
			return send_response(c,d,ans.str());
}

bool c::send_response__wallet_tx_decode(socket::peer_t* c, socket::datagram* d) {
	        string txb58;
			istringstream is(d->parse_string());
            is >> txb58;
			ostringstream ans;
			wallet_local_api::tx_decode(txb58,ans);
			return send_response(c,d,ans.str());
}

bool c::send_response__wallet_tx_check(socket::peer_t* c, socket::datagram* d) {
	        string txb58;
			istringstream is(d->parse_string());
            is >> txb58;
			ostringstream ans;
			wallet_local_api::tx_check(txb58,ans);
			return send_response(c,d,ans.str());
}

bool c::send_response__wallet_ping(socket::peer_t* c, socket::datagram* d) {
    //handled in process_work
   abort();
}

// pairing - master file: us/apitool/data/pairing
bool c::send_response__pairing_pair(socket::peer_t* c, socket::datagram* d) {
			istringstream is(d->parse_string());
	        pub_t pk;
            is >> pk;
            string name;
            is >> name;
			ostringstream ans;
			pairing_local_api::pair(pk,name,ans);
			return send_response(c,d,ans.str());
}

bool c::send_response__pairing_unpair(socket::peer_t* c, socket::datagram* d) {
	        pub_t pk;
			istringstream is(d->parse_string());
            is >> pk;
			ostringstream ans;
			pairing_local_api::unpair(pk,ans);
			return send_response(c,d,ans.str());
}

bool c::send_response__pairing_list_devices(socket::peer_t* c, socket::datagram* d) {
			ostringstream ans;
			pairing_local_api::list_devices(ans);

			return send_response(c,d,ans.str());
}





using namespace us::wallet::protocol;

bool c::process_work(socket::peer_t *c, datagram*d) {
	if (unlikely(d->service==us::wallet::protocol::wallet_ping)) {
		ostringstream ans;
		wallet_local_api::ping(ans);
		return send_response(c,d,ans.str());
    }

    if (b::process_work(c,d)) return true;

    if (static_cast<peer_t*>(c)->process_work(d)) return true;

    if (static_cast<auth::peer_t*>(c)->stage!=auth::peer_t::authorized) {
        cout << "not authorized" << endl;
        delete d;
        return true;
    }
	switch(d->service) {
        #include <us/api/apitool_generated__protocol_wallet-daemon_cpp_service_router>
		break;
		default: break;
	}
	return false;
}

socket::client* c::create_client(int sock) {
    auto p=new peer_t(sock);
    p->parent=this;
    return p;
}


bool c::authorize(const pub_t& p) const {
    return devices.authorize(p);
//cout << "Request for authorization " << p << "... ";
//    bool b=devices.authorize(p);
//cout << boolalpha << b << endl;
//    return b;
}

