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

 class encryption_protocol {
    

        //agreed params
        int agreedEphemeralKeySize = AES::DEFAULT_KEYLENGTH ;
        
        symmetric_encryption s_e;

            
        public:
            encryption_protocol(const keys::priv_t& privkeyA, const keys::pub_t& pubkeyB);
             ~encryption_protocol();
