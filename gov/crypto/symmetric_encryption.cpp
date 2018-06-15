#include "symmetric_encryption.h"

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

#include <cassert>


using namespace us::gov::crypto;

typedef us::gov::crypto::symmetric_encryption c;

c::symmetric_encryption(const keys::priv_t& priv_key_a, const keys::pub_t& pub_key_b){
    set_agreed_key_value(priv_key_a,pub_key_b);
}

void c::set_agreed_key_value(const keys::priv_t& priv_key_a, const keys::pub_t& pub_key_b){
    
    if(!secp256k1_ecdh(ec::instance.ctx,key_,&pub_key_b,&priv_key_a[0])){
        cerr << "Could not create shared secret";
    } 
}

const vector<unsigned char> c::encrypt(const vector<unsigned char>& plaintext){
    
    vector<unsigned char> ciphertext(size_iv + plaintext.size() + AES::BLOCKSIZE);
    
    //we need a new iv for each message that is encrypted with the same key.
    prng_.GenerateBlock(c::iv_, size_iv);

    GCM<AES>::Encryption enc;
    enc.SetKeyWithIV(key_, sizeof(key_), iv_, size_iv);

    ArraySink cs(&ciphertext[0], ciphertext.size());

    ArraySource(plaintext.data(), plaintext.size(), true, new AuthenticatedEncryptionFilter(enc, new Redirector(cs),false,tag_size_));
    
    // Set cipher text length now that its known, and append the iv
    /*size_t ciphertext_length= cs.TotalPutLength()+size_iv;
    ciphertext.resize(ciphertext_length);
    for(size_t i = 0; i < size_iv; i++){
        ciphertext[i+cs.TotalPutLength()] = iv_[i];
    }*/
    ciphertext.resize(cs.TotalPutLength());
    ciphertext.insert(ciphertext.end(), begin(iv_), end(iv_));
    return move(ciphertext);

}

void c::set_iv_from_ciphertext(const vector<unsigned char>& ciphertext){
    
    size_t iv_start = ciphertext.size() - size_iv;
    for(size_t i = 0; i < size_iv; i++){
        iv_[i] = ciphertext[i+iv_start];
    }
}

const vector<unsigned char> c::decrypt(const vector<unsigned char>& ciphertext){
    
    if(ciphertext.size()<size_iv){
        cerr << "The ciphertext does not have sufficient length to contain the iv (initialisation vector) which should have been appended.\n" << endl;
        return vector<unsigned char>();
    }
    vector<unsigned char> decryptedtext(ciphertext.size());
    set_iv_from_ciphertext(ciphertext);

    try{
        GCM< AES >::Decryption d;
        d.SetKeyWithIV( key_, sizeof(key_), iv_, size_iv );
       
        ArraySink sink(decryptedtext.data(), decryptedtext.size());
        AuthenticatedDecryptionFilter filter(d, new Redirector(sink),AuthenticatedDecryptionFilter::DEFAULT_FLAGS, tag_size_);
        ArraySource(ciphertext.data(), ciphertext.size()-size_iv, true, new Redirector(filter));

        // Set recovered text length now that its known
        decryptedtext.resize(sink.TotalPutLength());
    }
    catch(const CryptoPP::HashVerificationFilter::HashVerificationFailed& e){
        cerr << "Caught HashVerificationFailed...\n" << e.what() << "\n" << endl;
        return vector<unsigned char>();
    }
    catch(const CryptoPP::InvalidArgument& e ){
        cerr << "Caught InvalidArgument...\n" << e.what() << "\n" << endl;
        return vector<unsigned char>();
    }
    catch(const CryptoPP::Exception& e ){
        cerr << "Caught Exception...\n" << e.what() << "\n" << endl;
        return vector<unsigned char>();
    }

    return move(decryptedtext);
}


/*std::string c::decrypt(const std::string& ciphertext){
    
    set_iv_from_ciphertext(ciphertext);
    std::string plaintext;
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
}*/

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









