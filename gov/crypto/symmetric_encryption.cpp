#include "symmetric_encryption.h"

#include <iterator>
#include <secp256k1_ecdh.h>
#include <crypto++/cryptlib.h>
#include <crypto++/filters.h>
#include <crypto++/gcm.h>
#include <crypto++/aes.h>

using CryptoPP::GCM;
//using CryptoPP::HexEncoder;
//using CryptoPP::HexDecoder;
using CryptoPP::BufferedTransformation;
using CryptoPP::AuthenticatedSymmetricCipher;
using CryptoPP::Redirector;
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::ArraySink;
using CryptoPP::ArraySource;
using CryptoPP::AuthenticatedEncryptionFilter;
using CryptoPP::AuthenticatedDecryptionFilter;
using CryptoPP::AES;

using namespace us::gov::crypto;

typedef us::gov::crypto::symmetric_encryption c;

c::symmetric_encryption(const keys::priv_t& priv_key_a, const keys::pub_t& pub_key_b) {
    if (!ec::instance.generate_shared_key(key_, sizeof(key_), priv_key_a, pub_key_b)) {
		throw "Could not initialize encryption";
    }
}

c::symmetric_encryption(const vector<unsigned char>& shared_key) {
    if(shared_key.size()==key_size){
        for(size_t i = 0; i < key_size; i++){
            key_[i] = shared_key[i];
        }
    }
    else{
        throw "Could not initialize encryption. The key provided should be " + to_string(key_size) + " bytes.";
    } 
}

const vector<unsigned char> c::encrypt(const vector<unsigned char>& plaintext) {
    
    vector<unsigned char> ciphertext(iv_size + plaintext.size() + AES::BLOCKSIZE);
    try{
        //we need a new iv for each message that is encrypted with the same key.
        prng_.GenerateBlock(iv_, iv_size);

        GCM<AES>::Encryption enc;
        enc.SetKeyWithIV(key_, key_size, iv_, iv_size);

        ArraySink cs(&ciphertext[0], ciphertext.size());

        ArraySource(plaintext.data(), plaintext.size(), true, new AuthenticatedEncryptionFilter(enc, new Redirector(cs),false,tag_size));

        // Set cipher text length now that its known, and append the iv
        ciphertext.resize(cs.TotalPutLength());
        ciphertext.insert(ciphertext.end(), begin(iv_), end(iv_));
    }
    catch( CryptoPP::InvalidArgument& e )
    {
        cerr << "Caught InvalidArgument...\n" << e.what() << "\n" << endl;
        return vector<unsigned char>();
    }
    return move(ciphertext);

}

void c::set_iv_from_ciphertext(const vector<unsigned char>& ciphertext) {
    size_t iv_start = ciphertext.size() - iv_size;
    for(size_t i = 0; i < iv_size; i++){
        iv_[i] = ciphertext[i+iv_start];
    }
}

const vector<unsigned char> c::decrypt(const vector<unsigned char>& ciphertext) {
    if(ciphertext.size()<iv_size){
        return vector<unsigned char>();
    }
    vector<unsigned char> decryptedtext(ciphertext.size());
    set_iv_from_ciphertext(ciphertext);

    try{
        GCM< AES >::Decryption d;
        d.SetKeyWithIV( key_, key_size, iv_, iv_size );

        ArraySink sink(decryptedtext.data(), decryptedtext.size());
        AuthenticatedDecryptionFilter filter(d, new Redirector(sink),AuthenticatedDecryptionFilter::DEFAULT_FLAGS, tag_size);
        ArraySource(ciphertext.data(), ciphertext.size() - iv_size, true, new Redirector(filter));

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









