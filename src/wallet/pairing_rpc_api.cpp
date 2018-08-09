#include "pairing_rpc_api.h"
#include "protocol.h"
#include <sstream>
#include <us/gov/socket/datagram.h>

typedef us::wallet::pairing_rpc_api c;
using namespace us::wallet;
using namespace std;


c::pairing_rpc_api(const b::keys&k, const string& walletd_host, uint16_t walletd_port):b(k,walletd_host,walletd_port) {
}

c::~pairing_rpc_api() {
}

using namespace protocol;
#include <us/api/apitool_generated__functions_pairing_cpp_rpc-impl>

/*
void c::pair(const pub_t& pk, const string& name, ostream&os) {
	ostringstream si;
	si << pk << ' ' << name;
	ask(us::wallet::protocol::pair_query,si.str(),os);
}

void c::unpair(const pub_t& pk, ostream&os) {
	ostringstream si;
	si << pk;
	ask(us::wallet::protocol::unpair_query,si.str(),os);
}

void c::list_devices(ostream&os) {
	ask(us::wallet::protocol::list_devices_query,os);
}
*/
