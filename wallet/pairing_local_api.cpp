#include "pairing_local_api.h"
#include "protocol.h"
#include <sstream>
#include <us/gov/socket/datagram.h>
#include <us/gov/cash.h>

typedef us::wallet::pairing_local_api c;
using namespace us::wallet;
using namespace std;


c::pairing_local_api(const string& homedir): b(homedir) {
}

c::~pairing_local_api() {
}

void c::pair(const pub_t& pk, const string& name, ostream&os) {
    if (!pk.valid) {
        os << "Error: invalid public key.";
        return;
    }
    devices.pair(pk,name);
	os << "done.";
}

void c::unpair(const pub_t& pk, ostream&os) {
    devices.unpair(pk);
	os << "done.";
}

void c::list_devices(ostream&os) {
    devices.dump(os);
}

