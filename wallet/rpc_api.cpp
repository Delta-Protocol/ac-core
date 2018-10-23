#include "rpc_api.h"
#include "protocol.h"
#include <sstream>
#include <us/gov/socket.h>
#include <us/gov/cash.h>

using namespace us::wallet;
using namespace us;
using namespace std;


rpc_api::rpc_api(const gov::auth::peer_t::keys&k, 
                 const string& walletd_host, 
                 uint16_t walletd_port): 
                     m_walletd_host(walletd_host), 
                     m_walletd_port(walletd_port), gov::auth::peer_t(0), m_id(k) {
}

rpc_api::~rpc_api() {}

bool rpc_api::connect_walletd(ostream&os) {
    if (us::gov::socket::peer_t::connected()) 
        return true;
    auto r=connect(m_walletd_host, m_walletd_port,true);
    if (unlikely(!r.empty())) { 
        os << "Error. " << r;
        return false;
    }

    r=run_auth_responder();
    
    if (unlikely(!r.empty())) {
         os << r;
         return false;
    }
 
    if (!us::gov::socket::peer_t::connected()) {
         os << "Auth failed";
         return false;
    }

    m_connected_since=chrono::steady_clock::now();
    return true;
}

void rpc_api::ask(uint16_t service, ostream&os) {
    ask(service,"",os);
}

void rpc_api::ask(uint16_t service, const string& args, ostream&os) {
    if (!connect_walletd(os)) 
        return;

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

