#ifndef SYMMETRIC_ENCRYPTION
#define SYMMETRIC_ENCRYPTION

#include <iostream>

#include <cryptopp/osrng.h>
using CryptoPP::AutoSeededRandomPool;


#include <cryptopp/aes.h>
using CryptoPP::AES;

namespace us { namespace gov {
namespace crypto {
using namespace std;

    class symmetric_encryption {

        AutoSeededRandomPool prng;
        unsigned char key[ AES::DEFAULT_KEYLENGTH ];
        unsigned char iv[ AES::BLOCKSIZE ];

        //int encryptionCount = 0;
            
        public:
            symmetric_encryption();
             ~symmetric_encryption();

            void setKey(unsigned char* key, size_t length);
            void generateKey();
            void generateIV();
            
            bool encrypt(const string& plaintext, string& ciphertext, const int TAG_SIZE = 12);
            bool decrypt(const string& ciphertext, string& plaintext, const int TAG_SIZE = 12);
        };

    
}
}}
#endif


