#include "symmetric_encryption.h"

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


typedef us::gov::crypto::symmetric_encryption c;

c::symmetric_encryption(const keys::priv_t priv_key_a, const keys::pub_t pub_key_b){
    set_agreed_key_value(priv_key_a,pub_key_b);
}

void c::set_agreed_key_value(const keys::priv_t priv_key_a, const keys::pub_t pub_key_b)
{
    if(secp256k1_ecdh(ec::instance.ctx,key_,&pub_key_b,&priv_key_a[0])!=1)
    {
        cerr << "Could not create shared secret";
    } 
  
}

string c::encrypt(const string& plaintext)
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

string c::retrieve_ciphertext_and_set_iv(string iv_ciphertext)
{
    int s = sizeof(iv_);
    string ciphertext = iv_ciphertext.substr(s);
    
    for(int i = 0; i< s; i++){
        iv_[i]=iv_ciphertext[i];
    }
    
    return ciphertext;
}

string c::decrypt(const string& iv_ciphertext){
    
    string ciphertext = retrieve_ciphertext_and_set_iv(iv_ciphertext);
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








