#include "signed_data.h"
#include <cassert>

using namespace us::gov::engine;
using namespace std;

void signed_data::sign(const crypto::ec::keys& k) {
    m_pubkey=k.get_pubkey();
    string msg=message_to_sign();
    m_signature=crypto::ec::get_instance().sign_encode(k.get_privkey(),msg);
}

bool signed_data::verify() const {
    return crypto::ec::get_instance().verify(m_pubkey,message_to_sign(),m_signature);
}


