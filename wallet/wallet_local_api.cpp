#include "wallet_local_api.h"

#include <sstream>
#include <utility>
#include <string>

#include <us/gov/socket/datagram.h>
#include <us/gov/cash.h>
#include <us/gov/cash/tx.h>
#include <us/gov/socket/datagram.h>

#include "protocol.h"

using namespace us::wallet;
using namespace std;
using datagram=socket::datagram;
using us::gov::socket::datagram;

wallet_local_api::wallet_local_api(const string& homedir, 
                                   const string& backend_host, 
                                   uint16_t backend_port): 
                                   wallet(homedir, backend_host, backend_port) {
}

wallet_local_api::~wallet_local_api() {}

bool wallet_local_api::connect_backend(ostream&os) {
    if (m_endpoint.connected()) 
        return true;
    auto r=m_endpoint.connect(m_backend_host, m_backend_port,true);
    if (!r.empty()) {
        os << "Error. " << r;
        return false;
    }
    m_connected_since=chrono::steady_clock::now();
    return true;
}

void wallet_local_api::balance(bool detailed, ostream&os) {
    if (!connect_backend(os)) return;
    auto r=refresh(m_endpoint);
    if (unlikely(!r.empty())) {
        os << r;
        return;
    }
    if (detailed) {
        extended_balance(os);
    }else {
        os << wallet::balance();
    }
}

void wallet_local_api::list(bool showpriv, ostream& os) {     
    wallet::list(showpriv,os);
}

void wallet_local_api::new_address(ostream&os) {
    os << wallet::new_address();
}

void wallet_local_api::add_address(const crypto::ec::keys::priv_t& privkey, ostream&os) {
    os << wallet::add_address(privkey);
}

void wallet_local_api::transfer(const hash_t&addr, 
                                const cash_t&amount, 
                                ostream& os) {
    tx_make_p2pkh_input i;
    i.set_rcpt_addr(addr);
    i.set_amount(amount);
    i.set_fee(1);
    i.set_sigcode_inputs(cash::tx::sigcode_all);
    i.set_sigcode_outputs(cash::tx::sigcode_all);
    i.set_sendover(true);
    tx_make_p2pkh(i,os);
}

void wallet_local_api::tx_make_p2pkh(const tx_make_p2pkh_input&i,
                                     ostream& os) {
    if (!connect_backend(os)) 
        return;

    auto tx=wallet::tx_make_p2pkh(m_endpoint,i);
    if (tx.first.empty())
        os << *tx.second;
    else 
        os << tx.first;
}

void wallet_local_api::tx_sign(const string&txb58, 
                               cash::tx::sigcode_t sigcodei, 
                               cash::tx::sigcode_t sigcodeo, ostream& os) {
    if (!connect_backend(os)) 
        return;

    auto tx=wallet::tx_sign(m_endpoint, txb58, sigcodei, sigcodeo);
    if (tx.first.empty())
        os << *tx.second;
    else
        os << tx.first;
}

void wallet_local_api::tx_send(const string&txb58, ostream&os) {
    if (!connect_backend(os)) return;
    auto r=cash::tx::from_b58(txb58);
    if (unlikely(!r.first.empty())) {
        os << r.first;
        return;
    }
    string e=wallet::send(m_endpoint, *r.second);
    if (unlikely(!e.empty())) {
        os << e;
        return;
    }
    os << "Successfully sent :)";
}

void wallet_local_api::tx_decode(const string&txb58, ostream&os) {
    auto r=cash::tx::from_b58(txb58);
    if (unlikely(!r.first.empty())) {
        os << r.first;
        return;
    }
    r.second->write_pretty(os);
}

void wallet_local_api::tx_check(const string&txb58, ostream&os) {
    auto r=cash::tx::from_b58(txb58);

    if (unlikely(!r.first.empty())) {
        os << "Cannot decode base58";
        return;
    }

    const cash::tx& t=*r.second;
    bool fail=false;
    auto fee=t.check();

    if (fee<=0) {
        os << "Individual inputs and fees must be positive.";
        fail=true;
    }

    if (false){
        if (fail) 
            os << endl;
        os << "All in all you're just a nother brick in the wall.";
        fail=true;
    }

    if (fail){
        os << "This transaction looks ok.";
    }
}

void wallet_local_api::ping(ostream& os) {
    os << "I am responsive. The backend... ";
    if (!connect_backend(os)) {
        return;
    }
    auto r=m_endpoint.send_recv(new datagram(gov::protocol::gov_socket_ping,""),gov::protocol::gov_socket_pong);
    if (!r.first.empty()) {
        assert(r.second==0);
        os << "is not responding (Error): " << r.first;
        return;
    }
    datagram* d=r.second;
    os << "is also responsive, it said: " << d->parse_string();
    if (!r.first.empty()) {
        assert(r.second==0);
        os << "is not responding (Error): " << r.first;
        return;
    }
    delete d;
}



//---------impl of api functions that shall always be executed on the user pc and never query a remote server to do so

#include <sstream>
#include <us/gov/crypto.h>

void wallet_local_api::priv_key(const gov::crypto::ec::keys::priv_t& privkey, ostream&os) {
    if (!gov::crypto::ec::keys::verify(privkey)) {
        os << "The private key is incorrect.";
        return;
    }
    auto pub=gov::crypto::ec::keys::get_pubkey(privkey);
    os << "Public key: " << pub << endl;
    os << "Address: " << pub.compute_hash();
}

void wallet_local_api::gen_keys(ostream&os) {
    gov::crypto::ec::keys k=gov::crypto::ec::keys::generate();
    os << "Private key: " << k.get_privkey().to_b58() << endl;
    os << "Public key: " << k.get_pubkey().to_b58() << endl;
    os << "Address: " << k.get_pubkey().compute_hash();
}

void wallet_local_api::mine_public_key(const string& pattern, ostream& os) {
    while(true) {
        gov::crypto::ec::keys k=gov::crypto::ec::keys::generate();
        string pubkey=k.get_pubkey().to_b58();
        if (pubkey.find(pattern)!=string::npos) {
            os << "Private key: " << k.get_privkey().to_b58() << endl;
            os << "Public key: " << k.get_pubkey().to_b58() << endl;
            return;
        }
    }
}

