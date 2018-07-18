#include "peer_t.h"
#include "daemon.h"

typedef us::wallet::peer_t c;
using namespace std;

c::peer_t(int sock):b(sock) {
}

c::~peer_t() {
}
 
const c::keys& c::get_keys() const {
    return static_cast<wallet_daemon*>(parent)->id;
}

