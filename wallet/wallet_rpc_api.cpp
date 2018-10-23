#include "wallet_rpc_api.h"
#include "protocol.h"
#include <sstream>
#include <us/gov/socket/datagram.h>
#include <us/gov/cash.h>

using namespace us::wallet;
using namespace std;
typedef us::wallet::wallet_rpc_api c;

wallet_rpc_api::wallet_rpc_api(const rpc_api::keys&k, 
                               const string& walletd_host, 
                               uint16_t walletd_port):
                                   rpc_api(k, walletd_host, walletd_port) {
}

wallet_rpc_api::~wallet_rpc_api() {}

void wallet_rpc_api::ask_ping(ostream& os) {
    if (!connect_walletd(os)) 
        return;

    datagram* d=new datagram(us::wallet::protocol::wallet_ping,"");
    auto r=send_recv(d);
    if (unlikely(!r.first.empty())) {
        os << "(Error) " << r.first;
    }
    else {
        os << "Remote wallet says: " << r.second->parse_string();
        delete r.second;
    }
}

namespace {
ostream& operator << (ostream&os, bool v) {
    os << (v?1:0);
    return os;
}
}

using namespace protocol;
#include <us/api/apitool_generated__functions_wallet_cpp_rpc-impl> //APITOOL

