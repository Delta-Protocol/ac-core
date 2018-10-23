#include "peer_t.h"
#include "protocol.h"
#include "daemon.h"
#include <sstream>

#include <fstream>
#include <us/gov/crypto/base58.h>

using namespace us::gov::id;
using namespace std;

constexpr array<const char*,peer_t::num_stages> peer_t::stagestr;

peer_t::peer_t(int sock): socket::peer_t(sock) {}

peer_t::~peer_t() {}

string peer_t::connect(const string& host, uint16_t port, bool block) {
    auto r=b::connect(host,port,block);
    if (likely(r.empty())) {
        initiate_dialogue();
    }
    return r;
}

void peer_t::dump(ostream& os) const {
    os << "id: " << this << ' ' << get_pubkey()
       << " stage: " << stagestr[m_stage_peer] << endl;
}

void peer_t::process_request(datagram* d, const keys& mykeys) {
    string hello_msg=d->parse_string();
    string signature=crypto::ec::get_instance().sign_encode(mykeys.get_privkey(),hello_msg);
    m_msg=get_random_message();
    ostringstream os;
    assert(mykeys.get_pubkey().is_valid());
    os << mykeys.get_pubkey() << ' ' << signature << ' ' << m_msg;
    send(new datagram(protocol::gov_id_peer_challenge,os.str()));
}

void peer_t::process_peer_challenge(datagram* d, const keys& mykeys) {
    string data=d->parse_string();
    delete d;

    string hello_msg;
    pubkey_t peer_pubk;
    string peer_signature_der_b58;

    {
        istringstream is(data);
        is >> peer_pubk;
        is >> peer_signature_der_b58;
        is >> hello_msg;
    }

    if (unlikely(!peer_pubk.is_valid())) {
        m_stage_peer=verified_fail;
    }else if (likely(crypto::ec::get_instance().verify_not_normalized(
                                peer_pubk,m_msg,peer_signature_der_b58))) {
        m_stage_peer=verified;
        m_pubkey=peer_pubk;
    }else {
        m_stage_peer=verified_fail;
    }
    
    m_msg.clear();

    assert(mykeys.get_pubkey().is_valid());

    string signature_der_b58=crypto::ec::get_instance().sign_encode(mykeys.get_privkey(),hello_msg);
    ostringstream os;
    os << mykeys.get_pubkey() << ' ' << signature_der_b58;
    send(new datagram(protocol::gov_id_challenge_response,os.str()));
    verification_completed();
}

void peer_t::process_challenge_response(datagram* d) {
    string data=d->parse_string();
    delete d;

    pubkey_t peer_pubk;
    string peer_signature_der_b58;

    {
        istringstream is(data);
        is >> peer_pubk;
        is >> peer_signature_der_b58;
    }
	
    if (unlikely(!peer_pubk.is_valid())) {
        m_stage_peer=verified_fail;
    }else if (likely(crypto::ec::get_instance().verify_not_normalized(peer_pubk,m_msg,peer_signature_der_b58))) {
        m_stage_peer=peer_t::verified;
        m_pubkey=peer_pubk;
    }else {
        m_stage_peer=peer_t::verified_fail;
    }

    m_msg.clear();
    cout << "verif completed " << stagestr[m_stage_peer] << endl;

    verification_completed();
}

void peer_t::initiate_dialogue() {
    if (m_msg.empty()) { 
        m_msg=get_random_message();		
        send(new datagram(protocol::gov_id_request,m_msg));
    }
}

string peer_t::get_random_message() {
    vector<unsigned char> v;
    v.resize(20);
    ifstream f("/dev/urandom");
    f.read(reinterpret_cast<char*>(&v[0]),v.size());
    return crypto::b58::encode(v);
}

const peer_t::keys& peer_t::get_keys() const {
    assert(parent!=0);
    return static_cast<const daemon*>(parent)->get_keys();
}

bool peer_t::process_work(datagram*d) {
    switch(d->service) {
        case protocol::gov_id_request: 
            process_request(d,get_keys()); 
            break;
        case protocol::gov_id_peer_challenge: 
            process_peer_challenge(d,get_keys()); 
            break;
        case protocol::gov_id_challenge_response: 
            process_challenge_response(d); 
            break;
    default: 
        return false;
    }
    return true;
}

string peer_t::run_auth_responder() {
    assert(parent==0); 

    while(!program::_this.terminated) {
        if (m_stage_peer==verified || m_stage_peer==verified_fail) {
            break;
        }
        pair<string,datagram*> r=recv();
        if (unlikely(!r.first.empty())) {
            assert(r.second==0);
            ostringstream os;
            os << "Error. Peer not responding. " << r.first;
            return os.str();

        }

        if (likely(process_work(r.second))) {
                continue;
        }
        delete r.second;
        return "Error. Unrecognized datagram arrived";
    }
    return m_stage_peer==verified?"":"Error. Peer failed to demonstrate identity";
}
