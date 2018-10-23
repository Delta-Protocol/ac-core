#ifndef USGOV_d849d1a13db50de2445a34619f728b452c00b4067ae57c05409fe800ef621994
#define USGOV_d849d1a13db50de2445a34619f728b452c00b4067ae57c05409fe800ef621994

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


