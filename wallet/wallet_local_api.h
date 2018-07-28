#ifndef USGOV_b05bd77af998c823ed16a99e5cd5a0faa0ef03d000b57ee63148072bf9a65c79
#define USGOV_b05bd77af998c823ed16a99e5cd5a0faa0ef03d000b57ee63148072bf9a65c79

#include "wallet.h"

namespace us{ namespace wallet {
using namespace std;

    struct wallet_local_api: wallet::api, wallet {
        typedef wallet b;
        wallet_local_api(const string& homedir, const string& backend_host, uint16_t backend_port);
        virtual ~wallet_local_api();

    virtual void balance(bool detailed, ostream&os) override;
    virtual void list(bool showpriv, ostream&) override;

    virtual void new_address(ostream&os) override;
    using wallet::add_address;
    virtual void add_address(const priv_t& privkey, ostream&os) override;
    virtual void tx_make_p2pkh(const wallet::api::tx_make_p2pkh_input&, ostream&os) override;
    virtual void tx_sign(const string&txb58, sigcode_t inputs, sigcode_t outputs, ostream&os) override;
    virtual void tx_send(const string&txb58, ostream&os) override;
    virtual void tx_decode(const string&txb58, ostream&os) override;
    virtual void tx_check(const string&txb58, ostream&os) override;

    virtual void ping(ostream&) override;

    bool connect_backend(ostream&os);
    chrono::steady_clock::time_point connected_since;

    socket::peer_t endpoint;


    };


}}

#endif



