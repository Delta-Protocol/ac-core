#include "rpc_api.h"
#include "protocol.h"
#include <sstream>
#include <us/gov/socket.h>
#include <us/gov/cash.h>

typedef us::wallet::rpc_api c;
using namespace us::wallet;
using namespace std;


c::rpc_api(const b::keys&k, const string& walletd_host, uint16_t walletd_port):walletd_host(walletd_host), walletd_port(walletd_port), b(0), id(k) {
}

c::~rpc_api() {
}

bool c::connect_walletd(ostream&os) {
    if (us::gov::socket::peer_t::connected()) return true;
//cout << "---connecting" << endl;
    auto r=connect(walletd_host,walletd_port,true);
    if (unlikely(!r.empty())) { //connect(walletd_host,walletd_port,true)) {
//        cerr << r << endl; //"wallet: unable to connect to " << walletd_host << ":" << walletd_port << endl;
		os << "Error. " << r;
        return false;
    }

//cout << "---running auth" << endl;
    r=run_auth_responder();
//cout << "---run auth " << id::peer_t::stagestr[stage_me] << " " << id::peer_t::stagestr[stage_peer] << endl;
    if (unlikely(!r.empty())) {
         os << r;
         return false;
    }
    if (!us::gov::socket::peer_t::connected()) {
         os << "Auth failed";
         return false;
    }

    connected_since=chrono::steady_clock::now();
//cout << "---connected" << endl;
    return true;
}

void c::ask(uint16_t service, ostream&os) {
	ask(service,"",os);
}

//chrono::steady_clock::time_point

void c::ask(uint16_t service, const string& args, ostream&os) {
//cout << "asking " << service << " " << args << endl;
	if (!connect_walletd(os)) return;

	datagram* d=new datagram(service,args);
    auto r=send_recv(d); //TODO use expected answer
    if (unlikely(!r.first.empty())) {
        os << r.first;
    }
    else {
    	os << r.second->parse_string();
	    delete r.second;
    }
}

