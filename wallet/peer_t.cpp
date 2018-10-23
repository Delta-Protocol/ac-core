#include "peer_t.h"
#include "daemon.h"

using namespace us::wallet;
using namespace std;

peer_t::peer_t(int sock): auth::peer_t(sock) {}

peer_t::~peer_t() {}

const peer_t::keys& peer_t::get_keys() const {
    return static_cast<wallet_daemon*>(parent)->get_keys();
}

