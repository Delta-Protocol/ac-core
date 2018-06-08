#include "symmetric_encryption.h"

#include <string>
#include <iterator>

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
using CryptoPP::ArraySink;
using CryptoPP::ArraySource;
using CryptoPP::AuthenticatedEncryptionFilter;
using CryptoPP::AuthenticatedDecryptionFilter;

#include <cryptopp/gcm.h>
using CryptoPP::GCM;

#include <secp256k1_ecdh.h>


using namespace us::gov::crypto;
using namespace std;

typedef us::gov::crypto::symmetric_encryption c;

c::symmetric_encryption(const keys::priv_t& priv_key_a, const keys::pub_t& pub_key_b){
    set_agreed_key_value(priv_key_a,pub_key_b);
}

void c::set_agreed_key_value(const keys::priv_t& priv_key_a, const keys::pub_t& pub_key_b){
    
    if(secp256k1_ecdh(ec::instance.ctx,key_,&pub_key_b,&priv_key_a[0])!=1){
        cerr << "Could not create shared secret";
    } 
}

vector<unsigned char> c::encrypt(const vector<unsigned char>& plaintext){
    
    vector<unsigned char> ciphertext(sizeof(iv_) + plaintext.size() + AES::BLOCKSIZE);
    //we need a new iv for each message that is encrypted with the same key.
    prng_.GenerateBlock(c::iv_,sizeof(iv_));

    GCM<AES>::Encryption enc;
    enc.SetKeyWithIV(key_, sizeof(key_), iv_, sizeof(iv_));

    ArraySink cs(&ciphertext[0], ciphertext.size());

    ArraySource(plaintext.data(), plaintext.size(), true, new AuthenticatedEncryptionFilter(enc, new Redirector(cs),false,tag_size_));
    
    // Set cipher text length now that its known
    ciphertext.resize(cs.TotalPutLength());
}

string c::encrypt(const string& plaintext){
    string ciphertext;
    //we need a new iv for each message that is encrypted with the same key.
    prng_.GenerateBlock(c::iv_,sizeof(iv_));

    GCM<AES>::Encryption e;
    e.SetKeyWithIV(key_, sizeof(key_), iv_, sizeof(iv_));
    
    StringSource(plaintext, true, new AuthenticatedEncryptionFilter(e, new StringSink(ciphertext),false,tag_size_));
    prepend_iv_to_ciphertext(ciphertext);
    return ciphertext;
}


string c::retrieve_ciphertext_and_set_iv(string iv_ciphertext){
    size_t s= sizeof(iv_);
    string ciphertext = iv_ciphertext.substr(s);
    
    for(size_t i = 0; i< s; i++){
        iv_[i]=iv_ciphertext[i];
    }
    return ciphertext;
}

template<typename T>
void c::set_iv_from_ciphertext(T ciphertext){
    size_t s= sizeof(iv_);
    
    for(size_t i = 0; i< s; i++){
        iv_[i]=ciphertext[i];
    }
}

void c::prepend_iv_to_ciphertext(string& ciphertext){
    ciphertext = string(reinterpret_cast<char *>(iv_), sizeof(iv_)) + ciphertext;
}

vector<unsigned char> c::prepend_iv_to_ciphertext(vector<unsigned char> ciphertext){
   size_t s= sizeof(iv_);
    
    for(size_t i = 0; i< s; i++){
        ciphertext[i]=iv_[i];
    }

    return ciphertext;
}


vector<unsigned char> c::decrypt(const vector<unsigned char>& ciphertext){
    
    vector<unsigned char> recoveredtext(ciphertext.size());
    c::set_iv_from_ciphertext(ciphertext);

    try{
        GCM< AES >::Decryption d;
        d.SetKeyWithIV( key_, sizeof(key_), iv_, sizeof(iv_) );
       
        ArraySink rs(recoveredtext.data(), recoveredtext.size());

        StringSource(&ciphertext[sizeof(iv_)], ciphertext.size()-sizeof(iv_), true,new AuthenticatedDecryptionFilter(d, new Redirector(rs)));

        // Set recovered text length now that its known
        recoveredtext.resize(rs.TotalPutLength());


    }
    catch( CryptoPP::HashVerificationFilter::HashVerificationFailed& e ){
        cerr << "Caught HashVerificationFailed..." << endl;
        cerr << e.what() << endl;
        cerr << endl;
    }
    catch( CryptoPP::InvalidArgument& e ){
        cerr << "Caught InvalidArgument..." << endl;
        cerr << e.what() << endl;
        cerr << endl;
    }
    catch( CryptoPP::Exception& e ){
        cerr << "Caught Exception..." << endl;
        cerr << e.what() << endl;
        cerr << endl;
    }

    return recoveredtext;

}

string c::decrypt(const string& ciphertext){
    
    c::set_iv_from_ciphertext(ciphertext);
    string plaintext;
    try{
        GCM< AES >::Decryption d;
        d.SetKeyWithIV( key_, sizeof(key_), iv_, sizeof(key_) );
        
        AuthenticatedDecryptionFilter df( d, new StringSink( plaintext ), AuthenticatedDecryptionFilter::DEFAULT_FLAGS, tag_size_); 
        
        StringSource( &(ciphertext[sizeof(iv_)]), true, new Redirector( df) );

        // If the object does not throw, here's the only opportunity to check the data's integrity
        bool b = df.GetLastResult();
        assert( true == b );
    }
    catch( CryptoPP::HashVerificationFilter::HashVerificationFailed& e ){
        cerr << "Caught HashVerificationFailed..." << endl;
        cerr << e.what() << endl;
        cerr << endl;
    }
    catch( CryptoPP::InvalidArgument& e ){
        cerr << "Caught InvalidArgument..." << endl;
        cerr << e.what() << endl;
        cerr << endl;
    }
    catch( CryptoPP::Exception& e ){
        cerr << "Caught Exception..." << endl;
        cerr << e.what() << endl;
        cerr << endl;
    }
    return plaintext;
}

/*string c::decrypt(const string& iv_ciphertext){
    
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

}*/

/*string c::encrypt(const string& plaintext)
{
    //we need a new iv for each message that is encrypted with the same key.
    prng_.GenerateBlock(iv_,sizeof(iv_));


    string ciphertext;
    try
    {

        GCM< AES>::Encryption e;
        e.SetKeyWithIV( key_, sizeof(key_), iv_, sizeof(iv_) );
        
        StringSource( plaintext, true, new AuthenticatedEncryptionFilter( e, new StringSink( ciphertext ), false, TAG_SIZE)); 
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

    return ciphertext;
    
    
}*/









