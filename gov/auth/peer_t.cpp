#include "peer_t.h"
#include "protocol.h"
#include "daemon.h"


using namespace usgov::auth;
using namespace std;
typedef usgov::auth::peer_t c;

constexpr array<const char*,c::num_stages> c::stagestr;

c::peer_t(int sock): b(sock) {
}

c::~peer_t() {
}

void c::dump(ostream& os) const {
	os << this << "- I am (to peer): " << stagestr[stage_me] << "; Peer is (to me): " << stagestr[stage_peer] << endl;

}

void c::process_auth_request(datagram* d, const keys& mykeys) {
//cout << "AAA process_auth_request" << endl;
	string hello_msg=d->parse_string();
//cout << "process_auth_request " << hello_msg << endl;
	string signature=crypto::ec::instance.sign_encode(mykeys.priv,hello_msg);

	msg=get_random_message();		
	ostringstream os;
	os << mykeys.pub << ' ' << signature << ' ' << msg;
//cout << "AAA process_auth_request. sending auth_peer_challenge" << endl;
	send(new datagram(protocol::auth_peer_challenge,os.str()));
}

void c::process_auth_peer_challenge(datagram* d, const keys& mykeys) {
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

	if (crypto::ec::instance.verify(peer_pubk,msg,peer_signature_der_b58)) {
		stage_peer=verified;
		pubkey=peer_pubk;
	}
	else {
//cout << "FAIL 1" << endl;
		stage_peer=verified_fail;
		//TODO sleep random time before answering peer , timing attack
	}
	msg.clear();


	string signature_der_b58=crypto::ec::instance.sign_encode(mykeys.priv,hello_msg);
	ostringstream os;
	os << mykeys.pub << ' ' << signature_der_b58 << ' ' << stage_peer;
//cout << "AAA sending auth_challenge_response" << endl;
	send(new datagram(protocol::auth_challenge_response,os.str()));

}

void c::process_auth_challenge_response(datagram* d) {
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
	
	if (crypto::ec::instance.verify(peer_pubk,msg,peer_signature_der_b58)) {
		stage_peer=peer_t::verified;
		pubkey=peer_pubk;
	}
	else {
//cout << "FAIL 2" << endl;
		stage_peer=peer_t::verified_fail;
	}
	msg.clear();
	send(new datagram(protocol::auth_peer_status,(uint16_t) stage_peer));

//cout << "AAA calling verification_completed" << endl;
	verification_completed();
}

void c::process_auth_peer_status(datagram* d) {
	stage_me=(stage_t)(d->parse_uint16());
//cout << "AAA process_auth_peer_status " << stage_me << endl;
	delete d;

	verification_completed();
}

void c::do_actions() {
	if (stage_me==anonymous && msg.empty()) {  //if msg not empty we are carrying on the auth process already
		//cout << "auth: peer_t: send protocol::auth_request" << endl;
		msg=get_random_message();		

cout << "AAA sending auth_request" << endl;
		send(new datagram(protocol::auth_request,msg));
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

