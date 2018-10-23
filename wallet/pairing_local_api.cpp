#include "pairing_local_api.h"
#include <sstream>
#include <us/gov/socket/datagram.h>
#include <us/gov/cash.h>
#include "protocol.h"

using namespace us::wallet;
using namespace std;

pairing_local_api::pairing_local_api(const string& homedir): pairing(homedir) {}

pairing_local_api::~pairing_local_api() {}

void pairing_local_api::pair(const pub_t& pk, const string& name, ostream&os) {
    if (!pk.is_valid()) {
        os << "Error: invalid public key.";
        return;
    }
    pairing::pair(pk,name);
    os << "done.";
}

void pairing_local_api::unpair(const pub_t& pk, ostream&os) {
    pairing::unpair(pk);
    os << "done.";
}

void pairing_local_api::list_devices(ostream&os) {
    get_devices().dump(os);
}

