#ifndef USGOV_d849d1a13db50de2445a34619f728b452c00b4067ae57c05409fe800ef621994
#define USGOV_d849d1a13db50de2445a34619f728b452c00b4067ae57c05409fe800ef621994

#include <iostream>
#include <string>

#include <crypto++/osrng.h>
#include <crypto++/aes.h>
#include "ec.h"
#include <stdio.h>

namespace us { namespace gov {
namespace crypto {

using CryptoPP::AutoSeededRandomPool;
using CryptoPP::AES;
using namespace std;

typedef crypto::ec::keys keys;

    class symmetric_encryption {
        AutoSeededRandomPool prng_;
        
        const static size_t key_size = 16;
        const static size_t iv_size = 12;
        const int tag_size = 16;
        
        unsigned char key_[key_size];
        unsigned char iv_[iv_size];
        
        void set_iv_from_ciphertext(const vector<unsigned char>&);
        bool set_agreed_key_value(const keys::priv_t&, const keys::pub_t&);

        public:
            symmetric_encryption(const keys::priv_t&, const keys::pub_t&);
            symmetric_encryption(const vector<unsigned char>& );
            const vector<unsigned char> encrypt(const vector<unsigned char>&);
            const vector<unsigned char> decrypt(const vector<unsigned char>&);     //returns an empty vector<unsigned char> if decrypt is unsuccessful.
            
        };

}
}}
#endif


