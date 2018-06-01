#ifndef SYMMETRIC_ENCRYPTION
#define SYMMETRIC_ENCRYPTION

#include <iostream>

#include <cryptopp/osrng.h>
using CryptoPP::AutoSeededRandomPool;


#include <cryptopp/aes.h>
using CryptoPP::AES;

#include "ec.h"

#include <secp256k1_ecdh.h>

namespace us { namespace gov {
namespace crypto {

typedef crypto::ec::keys keys;

    class symmetric_encryption{

        AutoSeededRandomPool prng_;
        unsigned char key_[AES::DEFAULT_KEYLENGTH];
        unsigned char iv_[ AES::BLOCKSIZE ];
        const int TAG_SIZE = 12;
        void set_agreed_key_value(const keys::priv_t priv_key_A, const keys::pub_t pub_key_B);
        string retrieve_ciphertext_and_set_iv(string ivCiphertext);
        
        public:
            
            symmetric_encryption(const keys::priv_t priv_key_A, const keys::pub_t pub_key_B);
            
            string encrypt(const string& plaintext);
            string decrypt(const string& ciphertext);
        };

    
}
}}
#endif


