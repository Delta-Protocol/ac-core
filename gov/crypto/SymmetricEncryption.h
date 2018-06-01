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

    class SymmetricEncryption{

        AutoSeededRandomPool prng_;
        unsigned char key_[AES::DEFAULT_KEYLENGTH];
        unsigned char iv_[ AES::BLOCKSIZE ];
        const int TAG_SIZE = 12;
        void SetKey(unsigned char* key, size_t length);
        void GenerateKey();
        void SetAgreedKeyValue(const keys::priv_t privkA, const keys::pub_t pubkeyB);
        string RetrieveCiphertextAndSetIv(string ivCiphertext);
        
        public:
            
            SymmetricEncryption(const keys::priv_t privkeyA, const keys::pub_t pubkeyB);
            // ~symmetric_encryption();
            
            string Encrypt(const string& plaintext);
            string Decrypt(const string& ciphertext);
        };

    
}
}}
#endif


