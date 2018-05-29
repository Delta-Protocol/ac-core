#include "encryption_protocol.h"
#include <ec.h>
using keys::priv_t;
using keys::pub_t;

using namespace us::gov::crypto;
using namespace std;

typedef us::gov::crypto::encryption_protocol c;


c::encryption_protocol(const keys::priv_t& privkeyA, const keys::pub_t& pubkeyB){
   GetAgreedKeyValue(c::se_key, privkeyA,privkeyB);
   encryptionCount = 1;
}



void c::setKey(unsigned char* key, size_t length){
    
    // add logic to validate key and return false if fails.
    std::copy(key, key+length, c::key);
    
}

void c::generateKey(){
    
    c::prng.GenerateBlock( c::key, sizeof(c::key) );
   
}

void c::GetAgreedKeyValue(const keys::priv_t& privkA, const keys::pub_t& pubkeyB)
{
    *se_key = 12;
}









