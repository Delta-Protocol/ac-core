#include "peer_t.h"
#include "daemon.h"

typedef us::wallet::peer_t c;
using namespace std;

c::peer_t(int sock):b(sock) {
cout << "wallet: peer device id is 25hWPEdydrHRapxuh4DjW5K5W4mHkNefew1zZ63dkGuoc" << endl;
        stage_peer=verified;
        stage_me=verified;
        pubkey="25hWPEdydrHRapxuh4DjW5K5W4mHkNefew1zZ63dkGuoc";
}

c::~peer_t() {
}
 
const c::keys& c::get_keys() const {
    return static_cast<wallet_daemon*>(parent)->id;
}

