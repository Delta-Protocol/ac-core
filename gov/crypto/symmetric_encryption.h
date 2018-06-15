#ifndef USGOV_d849d1a13db50de2445a34619f728b452c00b4067ae57c05409fe800ef621994
#define USGOV_d849d1a13db50de2445a34619f728b452c00b4067ae57c05409fe800ef621994

#include <iostream>
#include <string>

#include <cryptopp/osrng.h>
using CryptoPP::AutoSeededRandomPool;


#include <cryptopp/aes.h>
using CryptoPP::AES;

#include "ec.h"

#include <stdio.h>

using namespace std;

namespace us { namespace gov {
namespace crypto {

typedef crypto::ec::keys keys;

    class symmetric_encryption{

        AutoSeededRandomPool prng_;
        unsigned char key_[AES::DEFAULT_KEYLENGTH];
        unsigned char iv_[ AES::BLOCKSIZE ];
        const size_t size_iv = sizeof(iv_);
        const int tag_size_ = 16;
        void set_agreed_key_value(const keys::priv_t&, const keys::pub_t&);
        void set_iv_from_ciphertext(const vector<unsigned char>&);
        
        public:
            symmetric_encryption(const keys::priv_t&, const keys::pub_t&);
            const vector<unsigned char> encrypt(const vector<unsigned char>&);
            const vector<unsigned char> decrypt(const vector<unsigned char>&);     //returns an empty vector<unsigned char> if decrypt is unsuccessful.

        };

    
}
}}
#endif


