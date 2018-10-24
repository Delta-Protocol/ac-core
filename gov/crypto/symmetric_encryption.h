#ifndef US_GOV_CRYPTO_SMMETRIC_ENRYPTION_H
#define US_GOV_CRYPTO_SMMETRIC_ENRYPTION_H

#include <iostream>
#include <string>

#include <crypto++/osrng.h>
#include "ec.h"
#include <stdio.h>

namespace us{ namespace gov{ namespace crypto{

using CryptoPP::AutoSeededRandomPool;

class symmetric_encryption {
public:
    symmetric_encryption(const ec::keys::priv_t&, const ec::keys::pub_t&);
    symmetric_encryption(const std::vector<unsigned char>& );
    const std::vector<unsigned char> encrypt(const std::vector<unsigned char>&);     
    const std::vector<unsigned char> decrypt(const std::vector<unsigned char>&);

private: 
    void set_iv_from_ciphertext(const std::vector<unsigned char>&);  
private:
    AutoSeededRandomPool m_prng_;
        
    const static size_t m_key_size = 16;
    const static size_t m_iv_size = 12;
    const int m_tag_size = 16;
        
    unsigned char m_key_[m_key_size];
    unsigned char m_iv_[m_iv_size];
};

}}}
#endif


