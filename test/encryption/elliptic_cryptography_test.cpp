#include "elliptic_cryptography_test.h"
#include <cassert>
#include <crypto++/aes.h>
#include <iostream>

using us::gov::crypto::ec;
using namespace std;
using CryptoPP::AES;

 bool test_elliptic_cryptography(){
    assert(test_shared_secret_generation());
    return true;
 }

 bool test_shared_secret_generation(){
    keys k1 = ec::keys::generate();
    keys k2 = ec::keys::generate();
    unsigned char shared_key1[AES::DEFAULT_KEYLENGTH];
    unsigned char shared_key2[AES::DEFAULT_KEYLENGTH];
    ec::instance.generate_shared_key(shared_key1, sizeof(shared_key1), k1.priv, k2.pub);
    ec::instance.generate_shared_key(shared_key2, sizeof(shared_key2), k2.priv, k1.pub);
   
    return equal(begin(shared_key1), std::end(shared_key1), std::begin(shared_key2));
 }