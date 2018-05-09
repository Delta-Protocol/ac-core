#include "daemon.h"
#include "protocol.h"

using namespace usgov;
using namespace std;
typedef usgov::wallet_daemon c;


string c::devices_t::default_name("my device");

c::wallet_daemon(uint16_t port, const string& home, const string&backend_host, uint16_t backend_port): b(port,2), local_api(home,backend_host,backend_port), devices(home) {
}

c::~wallet_daemon() {
}

c::devices_t::devices_t(const string& home) {
    file=home+"/d";
    load();
}


void c::devices_t::load() {
	lock_guard<mutex> lock(mx);
    load_();
}

void c::devices_t::save() const {
	lock_guard<mutex> lock(mx);
    save_();
}

void c::devices_t::load_() {
    clear();
    ifstream is(home+"/d");
    while (is.good()) {
        device d=device::from_stream(is);
        emplace(d.pub.hash(),move(d));
    }
}

void c::devices_t::save_() const {
    ofstream os(home+"/d");
    for (auto&i:*this) {
        i.second.to_stream(os);
        os << ' ';
    }
}

void c::device::to_stream(ostream&os) const {
    os << pub << ' ' << name;
}

c::device c::device::from_stream(istream&is) {
    device d;
    pub_t pk;
    is >> d.pub;
    is >> d.name;
    return move(d);
}

void c::devices_t::pair(const pub_t& pub, const string& name) {
	lock_guard<mutex> lock(mx);
    emplace(pub.hash(),device(pub,name));
    save_();
}

void c::devices_t::unpair(const pub_t& pub) {
	lock_guard<mutex> lock(mx);
    auto i=find(pub.hash());
    if (i==end()) return;
    erase(i);
    save_();
}

const string& c::devices_t::get_name(const pub_t& pub) {
	lock_guard<mutex> lock(mx);
    auto i=find(pub.hash());
    if (i==end()) return default_name;
    return i->second.name;
}

bool c::send_response(peer_t *c, datagram*d, const string& payload) {
	c->send(protocol::wallet::response,payload);
	this_thread::sleep_for(1s);
	delete d;
	return true;
}

bool c::process_work(peer_t *c, datagram*d) {
	switch(d->service) {
		case protocol::wallet::tx_make_p2pkh_query: {
			istringstream is(d->parse_string());
			wallet::tx_make_p2pkh_input i=wallet::tx_make_p2pkh_input::from_stream(is);
			ostringstream ans;
			local_api::tx_make_p2pkh(i,ans);
			return send_response(c,d,ans.str());
		}
		break;
		case protocol::wallet::add_address_query: {
			crypto::ec::keys::priv_t privkey;
			istringstream is(d->parse_string());
			is >> privkey;
			ostringstream ans;
			local_api::add_address(privkey,ans);
			return send_response(c,d,ans.str());
		}
		break;
		case protocol::wallet::new_address_query: {
			ostringstream ans;
			local_api::new_address(ans);
			return send_response(c,d,ans.str());
		}
		break;
		case protocol::wallet::dump_query: {
			ostringstream ans;
			local_api::dump(ans);
			return send_response(c,d,ans.str());
		}
		break;
		case protocol::wallet::balance_query: {
			bool detailed=d->parse_string()=="1";
			refresh();

			ostringstream ans;
			local_api::balance(detailed,ans);
			return send_response(c,d,ans.str());
		}
		break;
		case protocol::wallet::tx_sign_query: {
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
		case protocol::wallet::tx_send_query: {
	        string txb58;
			istringstream is(d->parse_string());
            is >> txb58;
			ostringstream ans;
			local_api::tx_send(txb58,ans);
			return send_response(c,d,ans.str());
		}
		break;
		case protocol::wallet::tx_decode_query: {
	        string txb58;
			istringstream is(d->parse_string());
            is >> txb58;
			ostringstream ans;
			local_api::tx_decode(txb58,ans);
			return send_response(c,d,ans.str());
		}
		break;
		case protocol::wallet::tx_check_query: {
	        string txb58;
			istringstream is(d->parse_string());
            is >> txb58;
			ostringstream ans;
			local_api::tx_check(txb58,ans);
			return send_response(c,d,ans.str());
		}
		break;
		default: break;
	}
	return false;
}
