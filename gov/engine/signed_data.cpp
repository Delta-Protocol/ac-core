#include "signed_data.h"
#include <cassert>

using namespace us::gov::engine;
using namespace std;

typedef us::gov::engine::signed_data c;


void c::sign(const crypto::ec::keys& k) {
	pubkey=k.pub;
	string msg=message_to_sign();
	signature=crypto::ec::instance.sign_encode(k.priv,msg);
//cout << "signed local_deltas " << endl << "--" << msg << "--" << signature << " " << pubkey << endl;
}

bool c::verify() const {
//cout << "verifying local_deltas " << endl << "--" << message_to_sign() << "--" << signature << " " << pubkey << endl;
	return crypto::ec::instance.verify(pubkey,message_to_sign(),signature);
}


