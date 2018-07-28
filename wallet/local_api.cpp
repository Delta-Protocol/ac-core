#include "local_api.h"
#include "protocol.h"
#include <sstream>
#include <us/gov/socket/datagram.h>
#include <us/gov/cash.h>

typedef us::wallet::local_api c;
using namespace us::wallet;
using namespace std;

using datagram=socket::datagram;

c::local_api(const string& homedir, const string& backend_host, uint16_t backend_port): wallet_local_api(homedir, backend_host, backend_port), pairing_local_api(homedir) {
}

c::~local_api() {
}


