#ifndef TEST_CRYPTO_FUNCTION_H  
#define TEST_CRYPTO_FUNCTION_H

#include <string>
#include <us/gov/crypto/ec.h>

    typedef us::gov::crypto::ec::keys keys;
    bool test_symmetric_encryption();
    bool test_encrypt_decrypt(std::string plaintext, keys::priv_t privA, keys::pub_t pubA,keys::priv_t privB, keys::pub_t pubB);


#endif
