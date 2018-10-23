#include "pairing_rpc_api.h"
#include <sstream>
#include <us/gov/socket/datagram.h>
#include "protocol.h"

typedef us::wallet::pairing_rpc_api c;
using namespace us::wallet;
using namespace std;


pairing_rpc_api::pairing_rpc_api(const rpc_api::keys&k, 
                                 const string& walletd_host, 
                                 uint16_t walletd_port):rpc_api(k,walletd_host,walletd_port) {
}

pairing_rpc_api::~pairing_rpc_api() {}

using namespace protocol;
#include <us/api/apitool_generated__functions_pairing_cpp_rpc-impl> //APITOOL

