#include "daemon.h"
#include "protocol.h"
#include "peer_t.h"

using namespace us::wallet;
using namespace std;
typedef us::wallet::wallet_daemon c;

c::wallet_daemon(const keys& k, uint16_t port, const string& home, const string&backend_host, uint16_t backend_port): b(port,2), local_api(home,backend_host,backend_port), id(k) {
    assert(!home.empty());
}

c::~wallet_daemon() {
}

bool c::send_error_response(peer_t *c, datagram*d, const string& error) {
	delete d;
//	this_thread::sleep_for(500ms); //TODO check if we can do it better
	c->send(new datagram(us::wallet::protocol::response,"E "+error));
//cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWwW Sleep 500ms"  << endl;
//	this_thread::sleep_for(500ms); //TODO check if we can do it better
	return true;
}

bool c::send_response(peer_t *c, datagram*d, const string& payload) {
	delete d;
//cout << "WDaemon sending response " << payload << endl;
//	this_thread::sleep_for(500ms); //TODO check if we can do it better
	c->send(new datagram(us::wallet::protocol::response,payload));
//cout << "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWWW Sleep 500ms"  << endl;
//	this_thread::sleep_for(500ms); //TODO check if we can do it better
	return true;
}

bool c::process_work(socket::peer_t *c0, datagram*d) {
    if (b::process_work(c0,d)) return true;

    peer_t* c=static_cast<peer_t*>(c0);
    switch(d->service) {
	case us::wallet::protocol::ping: {
		ostringstream ans;
		local_api::ping_wallet(ans);
		return send_response(c,d,ans.str());
	}
	break;
	case us::wallet::protocol::ping_gov: {
		ostringstream ans;
		local_api::ping_gov(ans);
		return send_response(c,d,ans.str());
	}
	break;
    }

    if (static_cast<auth::peer_t*>(c0)->stage!=auth::peer_t::authorized) {
        return false;
    }
	switch(d->service) {
		case us::wallet::protocol::tx_make_p2pkh_query: {
			istringstream is(d->parse_string());
			wallet::tx_make_p2pkh_input i=wallet::tx_make_p2pkh_input::from_stream(is);
            if (unlikely(is.fail())) {
                return send_error_response(c,d,"Unacceptable input.");
            }

			ostringstream ans;
			local_api::tx_make_p2pkh(i,ans);
			return send_response(c,d,ans.str());
		}
		break;
		case us::wallet::protocol::add_address_query: {
			crypto::ec::keys::priv_t privkey;
			istringstream is(d->parse_string());
			is >> privkey;
			ostringstream ans;
			local_api::add_address(privkey,ans);
			return send_response(c,d,ans.str());
		}
		break;
		case us::wallet::protocol::new_address_query: {
			ostringstream ans;
			local_api::new_address(ans);
			return send_response(c,d,ans.str());
		}
		break;
		case us::wallet::protocol::dump_query: {
			ostringstream ans;
			local_api::dump(ans);
			return send_response(c,d,ans.str());
		}
		break;
		case us::wallet::protocol::balance_query: {
			bool detailed=d->parse_string()=="1";
			ostringstream ans;
   			local_api::balance(detailed,ans);
//cout << "ANSWERING : " << ans.str() << endl;
//cout << "In 5 seconds" << endl;
//this_thread::sleep_for(5s);
//cout << "now" << endl;
			return send_response(c,d,ans.str());
		}
		break;
		case us::wallet::protocol::tx_sign_query: {
	        string txb58;
            cash::tx::sigcode_t sci;
            cash::tx::sigcode_t sco;
			istringstream is(d->parse_string());
            is >> txb58;
            is >> sci;
            is >> sco;
			ostringstream ans;
			local_api::tx_sign(txb58,sci,sco,ans);
			return send_response(c,d,ans.str());
		}
		break;
		case us::wallet::protocol::tx_send_query: {
	        string txb58;
			istringstream is(d->parse_string());
            is >> txb58;
			ostringstream ans;
			local_api::tx_send(txb58,ans);
			return send_response(c,d,ans.str());
		}
		break;
		case us::wallet::protocol::tx_decode_query: {
	        string txb58;
			istringstream is(d->parse_string());
            is >> txb58;
			ostringstream ans;
			local_api::tx_decode(txb58,ans);
			return send_response(c,d,ans.str());
		}
		break;
		case us::wallet::protocol::tx_check_query: {
	        string txb58;
			istringstream is(d->parse_string());
            is >> txb58;
			ostringstream ans;
			local_api::tx_check(txb58,ans);
			return send_response(c,d,ans.str());
		}
		break;
		case us::wallet::protocol::pair_query: {
			istringstream is(d->parse_string());
	        pub_t pk;
            is >> pk;
            string name;
            is >> name;
			ostringstream ans;
			local_api::pair(pk,name,ans);
			return send_response(c,d,ans.str());
		}
		break;
		case us::wallet::protocol::unpair_query: {
	        pub_t pk;
			istringstream is(d->parse_string());
            is >> pk;
			ostringstream ans;
			local_api::unpair(pk,ans);
			return send_response(c,d,ans.str());
		}
		break;
		case us::wallet::protocol::list_devices_query: {
			ostringstream ans;
			local_api::list_devices(ans);

			return send_response(c,d,ans.str());
		}
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
cout << "Request for authorization " << p << endl;
    return devices.authorize(p);
}

