#ifndef USGOV_d849d1a13db50de2445a34619f728b452c00b4067ae57c05409fe800ef621994
#define USGOV_d849d1a13db50de2445a34619f728b452c00b4067ae57c05409fe800ef621994

#include <iostream>

#include <cryptopp/osrng.h>
using CryptoPP::AutoSeededRandomPool;


#include <cryptopp/aes.h>
using CryptoPP::AES;

#include "ec.h"

#include <stdio.h>


namespace us { namespace gov {
namespace crypto {

typedef crypto::ec::keys keys;

    class symmetric_encryption{

        AutoSeededRandomPool prng_;
        unsigned char key_[AES::DEFAULT_KEYLENGTH];
        unsigned char iv_[ AES::BLOCKSIZE ];
        const size_t size_iv = AES::BLOCKSIZE;
        const int tag_size_ = 12;
        void set_agreed_key_value(const keys::priv_t&, const keys::pub_t&);
        string retrieve_ciphertext_and_set_iv(string);
        template<typename T>
        void set_iv_from_ciphertext(T);
        string prepend_iv_to_ciphertext(string&);
        vector<unsigned char> prepend_iv_to_ciphertext(vector<unsigned char>);
        
        public:
            
            symmetric_encryption(const keys::priv_t&, const keys::pub_t&);
            vector<unsigned char> encrypt(const vector<unsigned char>&);
            vector<unsigned char> decrypt(const vector<unsigned char>&);
            string encrypt(const string&);
            string decrypt(const string&);
        };

    
}
}}
#endif


