#include "SymmetricEncryption.h"

#include <iostream>
using std::cout;
using std::cerr;

#include <string>
using std::string;

#include <iterator>
using std::begin;
using std::end;

#include <cryptopp/hex.h>
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;

#include <cryptopp/cryptlib.h>
using CryptoPP::BufferedTransformation;
using CryptoPP::AuthenticatedSymmetricCipher;

#include <cryptopp/filters.h>
using CryptoPP::Redirector;
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::AuthenticatedEncryptionFilter;
using CryptoPP::AuthenticatedDecryptionFilter;

#include <cryptopp/gcm.h>
using CryptoPP::GCM;





using namespace us::gov::crypto;


typedef us::gov::crypto::SymmetricEncryption c;

c::SymmetricEncryption(const keys::priv_t privkeyA, const keys::pub_t pubkeyB){
    SetAgreedKeyValue(privkeyA,pubkeyB);
}

void c::SetAgreedKeyValue(const keys::priv_t privkeyA, const keys::pub_t pubkeyB)
{
    
    if(secp256k1_ecdh(ec::instance.ctx,key_,&pubkeyB,&privkeyA[0])!=1)
    {
        cerr << "Could not create shared secret";
    } 
    //implement later
     //std::copy(key, key+length, c::key);
}

string c::Encrypt(const string& plaintext)
{
    //we need a new iv for each message that is encrypted with the same key.
    prng_.GenerateBlock(iv_,sizeof(iv_));


    string ciphertext;
    try
    {
        cout << "plain text: " << plaintext << endl;

        GCM< AES>::Encryption e;
        e.SetKeyWithIV( key_, sizeof(key_), iv_, sizeof(iv_) );
        
        StringSource( plaintext, true,
            new AuthenticatedEncryptionFilter( e,
                new StringSink( ciphertext ), false, TAG_SIZE
            ) 
        ); 
    }
    catch( CryptoPP::InvalidArgument& e )
    {
        cerr << "Caught InvalidArgument..." << endl;
        cerr << e.what() << endl;
        cerr << endl;
    }
    catch( CryptoPP::Exception& e )
    {
        cerr << "Caught Exception..." << endl;
        cerr << e.what() << endl;
        cerr << endl;
    }
    //maybe iv should be appended using filter

    return string(reinterpret_cast<char *>(iv_), sizeof(iv_)) + ciphertext;
    
    
}

string c::RetrieveCiphertextAndSetIv(string ivCiphertext)
{
    int s = sizeof(iv_);
    string ciphertext = ivCiphertext.substr(s);
    
    for(int i = 0; i< s; i++){
        iv_[i]=ivCiphertext[i];
    }
    
    return ciphertext;
}

string c::Decrypt(const string& ivCiphertext){
    
    string ciphertext = RetrieveCiphertextAndSetIv(ivCiphertext);
    string plaintext;
    try
    {
        GCM< AES >::Decryption d;
        d.SetKeyWithIV( key_, sizeof(key_), iv_, sizeof(key_) );
        // d.SpecifyDataLengths( 0, cipher.size()-TAG_SIZE, 0 );

        AuthenticatedDecryptionFilter df( d,
            new StringSink( plaintext ),
            AuthenticatedDecryptionFilter::DEFAULT_FLAGS,
            TAG_SIZE
        ); // AuthenticatedDecryptionFilter

        // The StringSource dtor will be called immediately
        //  after construction below. This will cause the
        //  destruction of objects it owns. To stop the
        //  behavior so we can get the decoding result from
        //  the DecryptionFilter, we must use a redirector
        //  or manually Put(...) into the filter without
        //  using a StringSource.
        StringSource( ciphertext, true,
            new Redirector( df) //PASS EVERYTHING
        ); // StringSource

        // If the object does not throw, here's the only
        //  opportunity to check the data's integrity
        bool b = df.GetLastResult();
        //assert( true == b );

    }
    catch( CryptoPP::HashVerificationFilter::HashVerificationFailed& e )
    {
        cerr << "Caught HashVerificationFailed..." << endl;
        cerr << e.what() << endl;
        cerr << endl;
    }
    catch( CryptoPP::InvalidArgument& e )
    {
        cerr << "Caught InvalidArgument..." << endl;
        cerr << e.what() << endl;
        cerr << endl;
    }
    catch( CryptoPP::Exception& e )
    {
        cerr << "Caught Exception..." << endl;
        cerr << e.what() << endl;
        cerr << endl;
    }
    return plaintext;

}








