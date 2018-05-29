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
            int GetKeySize();
            
            string encrypt(const string& plaintext, const unsigned char* key, const unsigned char* iv, const int TAG_SIZE = 12);
            string decrypt(const string& ciphertext, const unsigned char* key, const unsigned char* iv, const int TAG_SIZE = 12);
        };

    
}
}}
#endif


