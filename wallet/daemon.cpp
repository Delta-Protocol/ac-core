#include "daemon.h"
#include "protocol.h"

using namespace usgov;
using namespace std;
typedef usgov::wallet_daemon c;


string c::devices_t::default_name("my device");

c::wallet_daemon(uint16_t port, const string& home, const string&backend_host, uint16_t backend_port): b(port,2), wallet(home), backend_port(backend_port), backend_host(backend_host), devices(home) {
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


bool c::process_work(peer_t *c, datagram*d) {
	switch(d->service) {
		case protocol::wallet::seq_query: {
        }
        break;
		case protocol::wallet::balance_query: {
            

			refresh(backend_host, backend_port);
			ostringstream msg;
			msg << balance();
			c->send(protocol::wallet::balance_response,msg.str());
			this_thread::sleep_for(1s);
			delete d;
			return true;
		}
		break;
		default: break;
	}
	return false;
}
