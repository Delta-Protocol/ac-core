#include "daemon.h"
#include "protocol.h"

using namespace us::wallet;
using namespace std;
typedef us::wallet::wallet_daemon c;



c::wallet_daemon(uint16_t port, const string& home, const string&backend_host, uint16_t backend_port): b(port,2), local_api(home,backend_host,backend_port) {
}

c::~wallet_daemon() {
}

bool c::send_error_response(peer_t *c, datagram*d, const string& error) {
	c->send(us::wallet::protocol::response,"E "+error);
	this_thread::sleep_for(500ms); //TODO check if we can do it better
	delete d;
	return true;
}

bool c::send_response(peer_t *c, datagram*d, const string& payload) {
	c->send(us::wallet::protocol::response,payload);
	this_thread::sleep_for(500ms); //TODO check if we can do it better
	delete d;
	return true;
}

bool c::process_work(peer_t *c, datagram*d) {
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
			refresh();

			ostringstream ans;
			local_api::balance(detailed,ans);
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
/*
		case us::wallet::protocol::nova_move: {
			istringstream is(d->parse_string());
			wallet::nova_move_input i=wallet::nova_move_input::from_stream(is);
            if (unlikely(is.fail())) {
                return send_error_response(c,d,"Unacceptable input.");
            }

			ostringstream ans;
			local_api::nova_move(i,ans);
			return send_response(c,d,ans.str());
        }
        break;
		case us::wallet::protocol::nova_track: {
			istringstream is(d->parse_string());
			wallet::nova_track_input i=wallet::nova_track_input::from_stream(is);
            if (unlikely(is.fail())) {
                return send_error_response(c,d,"Unacceptable input.");
            }

			ostringstream ans;
			local_api::nova_track(i,ans);
			return send_response(c,d,ans.str());
        }
        break;
		case us::wallet::protocol::nova_query: {
			string s=d->parse_string();
            istringstream is(s);
            nova::hash_t h;
            is >> h;
			ostringstream ans;
			local_api::nova_query(h,ans);
			return send_response(c,d,ans.str());
        }
        break;
		case us::wallet::protocol::nova_mempool: {
			ostringstream ans;
			local_api::nova_mempool(ans);
			return send_response(c,d,ans.str());
        }
        break;
*/
		default: break;
	}
	return false;
}

