#include "peer_t.h"
#include "protocol.h"
#include <sstream>

#include "daemon.h"

using namespace us::gov::id;
using namespace std;

typedef us::gov::id::peer_t c;

constexpr array<const char*,c::num_stages> c::stagestr;

c::peer_t(int sock): b(sock) {
}

c::~peer_t() {
}

string c::connect(const string& host, uint16_t port, bool block) {
    auto r=b::connect(host,port,block);
    if (likely(r.empty())) {
        initiate_dialogue();
    }
    return r;
}


void c::dump(ostream& os) const {
	os << "id: " << this << ' ' << pubkey << " stage: " << stagestr[stage_peer] << endl;
}

void c::process_request(datagram* d, const keys& mykeys) {
	string hello_msg=d->parse_string();
//cout << "ID signing with priv " << mykeys.priv << " - " << mykeys.pub << " " << hello_msg << endl;

	string signature=crypto::ec::instance.sign_encode(mykeys.priv,hello_msg);
//cout << "ID: process_request, msg2sign=" << hello_msg << " created signature=" << signature << endl;
	msg=get_random_message();
	ostringstream os;
    assert(mykeys.pub.valid);
	os << mykeys.pub << ' ' << signature << ' ' << msg;
//cout << "ID: process_request. sending id_peer_challenge: " << os.str() << endl;
	send(new datagram(protocol::gov_id_peer_challenge,os.str()));
}

void c::process_peer_challenge(datagram* d, const keys& mykeys) {
	string data=d->parse_string();
//cout << "ID: process_peer_challenge. received data: " << data << endl;
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

//cout << "ID: peer_signature_der_b58 " << peer_signature_der_b58 << endl;
//cout << "ID: peer_pubk " << peer_pubk << endl;

	if (unlikely(!peer_pubk.valid)) {
		stage_peer=verified_fail;
//cout << "ID: peer_pubk invalid" << endl;
	}
	else if (likely(crypto::ec::instance.verify_not_normalized(peer_pubk,msg,peer_signature_der_b58))) {
		stage_peer=verified;
		pubkey=peer_pubk;
//cout << "ID: peer_pubk ok" << endl;
	}
	else {
		stage_peer=verified_fail;
//cout << "ID: peer_pubk and signature verif fail" << endl;
		//TODO sleep random time before answering peer , timing attack
	}
	msg.clear();

	assert(mykeys.pub.valid);

//cout << "ID signing with priv " << mykeys.priv << " - " << mykeys.pub << endl;

	string signature_der_b58=crypto::ec::instance.sign_encode(mykeys.priv,hello_msg);
	ostringstream os;
	os << mykeys.pub << ' ' << signature_der_b58; // << ' ' << stage_peer;
//cout << "ID sending id_challenge_response " << os.str() << endl;
//while(true) {
	send(new datagram(protocol::gov_id_challenge_response,os.str()));
//    this_thread::sleep_for(1s);
//}


//cout << "verif completed " << stagestr[stage_peer] << endl;
    verification_completed();

}

void c::process_challenge_response(datagram* d) {
	string data=d->parse_string();
//cout << "ID: process_challenge_response " << data << endl;
	delete d;

	pubkey_t peer_pubk;
	string peer_signature_der_b58;
	//string sveredict;

	{
	istringstream is(data);
	is >> peer_pubk;
	is >> peer_signature_der_b58;
	//is >> sveredict;
	}
//cout << "ID: process_challenge_response received peer_signature_der_b58 " << peer_signature_der_b58 << endl;
//cout << "ID: process_challenge_response received peer_pubk " << peer_pubk << endl;
//cout << "ID: process_challenge_response received veredict " << sveredict << endl;
	//stage_me=(stage_t)(stoi(sveredict));
	
	if (unlikely(!peer_pubk.valid)) {
		stage_peer=verified_fail;
//cout << "ID: process_challenge_response FAIL1 " << endl;
	}
	else if (likely(crypto::ec::instance.verify_not_normalized(peer_pubk,msg,peer_signature_der_b58))) {
		stage_peer=peer_t::verified;
		pubkey=peer_pubk;
//cout << "ID: process_challenge_response peer pubkey " << pubkey << endl;
//cout << "ID: Peer pubkey is " << pubkey << endl;
	}
	else {
//cout << "ID: process_challenge_response FAIL2 " << endl;
		stage_peer=peer_t::verified_fail;
	}
	msg.clear();
	//send(new datagram(protocol::gov_id_peer_status,(uint16_t) stage_peer));

//cout << "ID: process_challenge_response  " << pubkey << endl;
//cout << "ID: process_challenge_response calling verification_completed" << endl;
cout << "verif completed " << stagestr[stage_peer] << endl;

	verification_completed();
}
/*
void c::process_peer_status(datagram* d) {
	stage_me=(stage_t)(d->parse_uint16());
//cout << "ID: process_peer_status " << stage_me << endl;
	delete d;

//cout << "ID: calling verification_completed" << endl;
	verification_completed();
}
*/
void c::initiate_dialogue() {
//cout << "ID: initiate_dialogue" << endl;
	if (msg.empty()) {  //if msg not empty we are carrying on the wauth process already
		msg=get_random_message();		
//cout << "ID: sending id_request" << endl;
		send(new datagram(protocol::gov_id_request,msg));
	}
}
/*
#include <iomanip>
#include <vector>
string c::to_string(const vector<unsigned char>& data) {
	ostringstream os;
	for (auto i:data) os << hex << setfill('0') << setw(2) << (int)i;
	return os.str();
}
*/

#include <fstream>
#include <us/gov/crypto/base58.h>

string c::get_random_message() {
//return "DUMMY-MSG";  //uncomment for deterministic debugging
	vector<unsigned char> v;
	v.resize(20);
    ifstream f("/dev/urandom");
    f.read(reinterpret_cast<char*>(&v[0]),v.size());
	return crypto::b58::encode(v);
}

const c::keys& c::get_keys() const {
    assert(parent!=0);
    return static_cast<const daemon*>(parent)->get_keys();
}


bool c::process_work(datagram*d) {
    switch(d->service) {
        case protocol::gov_id_request: process_request(d,get_keys()); break;
        case protocol::gov_id_peer_challenge: process_peer_challenge(d,get_keys()); break;
        case protocol::gov_id_challenge_response: process_challenge_response(d); break;
    default: return false;
    }
    return true;
}

string c::run_auth_responder() {
    assert(parent==0); //this object should not be managed by a daemon to use this function

    while(!program::_this.terminated) {
        if (stage_peer==verified || stage_peer==verified_fail) {
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
//    cout << "aaa " << (stage_peer==verified) << endl;
    return stage_peer==verified?"":"Error. Peer failed to demonstrate identity";
}
