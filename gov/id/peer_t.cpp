#include "peer_t.h"
#include "protocol.h"
#include "daemon.h"

using namespace us::gov::id;
using namespace std;

typedef us::gov::id::peer_t c;

constexpr array<const char*,c::num_stages> c::stagestr;

c::peer_t(int sock): b(sock) {
}

c::~peer_t() {
}

void c::on_connect() { //called only on the initiator, the caller side.
//cout << "AUTH ONCONNECT" << endl;
    b::on_connect();
//cout << "INITIATE DIALOGUE" << endl;
    initiate_dialogue();
}

void c::dump(ostream& os) const {
	os << this << "- I am (to peer): " << stagestr[stage_me] << "; Peer is (to me): " << stagestr[stage_peer] << endl;

}

void c::process_request(datagram* d, const keys& mykeys) {
//cout << "AAA process_auth_request" << endl;
	string hello_msg=d->parse_string();
//cout << "process_auth_request " << hello_msg << endl;
	string signature=crypto::ec::instance.sign_encode(mykeys.priv,hello_msg);

	msg=get_random_message();
	ostringstream os;
assert(mykeys.pub.valid);
	os << mykeys.pub << ' ' << signature << ' ' << msg;
//cout << "AAA process_auth_request. sending auth_peer_challenge" << endl;
	send(new datagram(protocol::id_peer_challenge,os.str()));
}

void c::process_peer_challenge(datagram* d, const keys& mykeys) {
//cout << "AAA process_auth_peer_challenge" << endl;
	string data=d->parse_string();
//cout << "process_auth_peer_challenge " << data << endl;
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

//cout << "peer_signature_der_b58 " << peer_signature_der_b58 << endl;
//cout << "peer_pubk " << peer_pubk << endl;

	if (unlikely(!peer_pubk.valid)) {
		stage_peer=verified_fail;
	}
	else if (likely(crypto::ec::instance.verify(peer_pubk,msg,peer_signature_der_b58))) {
		stage_peer=verified;
		pubkey=peer_pubk;
	}
	else {
//cout << "FAIL 1" << endl;
		stage_peer=verified_fail;
		//TODO sleep random time before answering peer , timing attack
	}
	msg.clear();

	assert(mykeys.pub.valid);

	string signature_der_b58=crypto::ec::instance.sign_encode(mykeys.priv,hello_msg);
	ostringstream os;
	os << mykeys.pub << ' ' << signature_der_b58 << ' ' << stage_peer;
//cout << "AAA sending auth_challenge_response" << endl;
	send(new datagram(protocol::id_challenge_response,os.str()));

}

void c::process_challenge_response(datagram* d) {
//cout << "AAA process_auth_challenge_response" << endl;
	string data=d->parse_string();
//cout << "process_auth_challenge_response " << data << endl;
	delete d;

	pubkey_t peer_pubk;
	string peer_signature_der_b58;
	string sveredict;

	{
	istringstream is(data);
	is >> peer_pubk;
	is >> peer_signature_der_b58;
	is >> sveredict;
	}
//cout << "peer_signature_der_b58 " << peer_signature_der_b58 << endl;
	stage_me=(stage_t)(stoi(sveredict));
	
	if (unlikely(!peer_pubk.valid)) {
		stage_peer=verified_fail;
	}
	else if (likely(crypto::ec::instance.verify(peer_pubk,msg,peer_signature_der_b58))) {
		stage_peer=peer_t::verified;
		pubkey=peer_pubk;
//cout << "Peer pubkey is " << pubkey << endl;
	}
	else {
//cout << "FAIL 2" << endl;
		stage_peer=peer_t::verified_fail;
	}
	msg.clear();
	send(new datagram(protocol::id_peer_status,(uint16_t) stage_peer));

//cout << "AAA calling verification_completed" << endl;
	verification_completed();
}

void c::process_peer_status(datagram* d) {
	stage_me=(stage_t)(d->parse_uint16());
//cout << "AAA process_auth_peer_status " << stage_me << endl;
	delete d;

//cout << "A2A calling verification_completed" << endl;
	verification_completed();
}

void c::initiate_dialogue() {
//cout << "initiate_dialogue" << endl;
	if (stage_me==anonymous && msg.empty()) {  //if msg not empty we are carrying on the wauth process already
		//cout << "auth: peer_t: send protocol::auth_request" << endl;
		msg=get_random_message();		

//cout << "AAA sending auth_request" << endl;
		send(new datagram(protocol::id_request,msg));
	}
}

#include <iomanip>
#include <vector>
string c::to_string(const vector<unsigned char>& data) {
	ostringstream os;
	for (auto i:data) os << hex << setfill('0') << setw(2) << (int)i;
	return os.str();
}

#include <fstream>
string c::get_random_message() {
	vector<unsigned char> v;
	v.resize(20);
    ifstream f("/dev/urandom");
    f.read(reinterpret_cast<char*>(&v[0]),v.size());
	return to_string(v);
 }


const c::keys& c::get_keys() const {
    assert(parent!=0);
    return static_cast<const daemon*>(parent)->get_keys();
}

bool c::process_work(datagram*d) {
    if (b::process_work(d)) return true;

    switch(d->service) {
        case protocol::id_request: process_request(d,get_keys()); break;
        case protocol::id_peer_challenge: process_peer_challenge(d,get_keys()); break;
        case protocol::id_challenge_response: process_challenge_response(d); break;
        case protocol::id_peer_status: process_peer_status(d); break;
    default: return false;
    }
    return true;
}


string c::run_auth() {
    assert(parent==0); //this object should not be managed by a daemon to use this function

    while(!program::_this.terminated) {
        if (stage_peer==verified || stage_peer==verified_fail) {
        if (stage_me==verified || stage_me==verified_fail) {
            break;
        }
        }
        pair<string,datagram*> r=recv(); //complete_datagram(10);
        if (unlikely(!r.first.empty())) {
            assert(r.second==0);
            return "Error. Peer not responding";
        }

        if (likely(process_work(r.second))) {
                continue;
        }
        delete r.second;
        return "Error. Unrecognized datagram arrived";
    }
    return stage_peer==verified?"":"Error. Peer failed to demonstrate identity";
 }
