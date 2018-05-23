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
using namespace std;

typedef us::gov::crypto::symmetric_encryption c;

c::symmetric_encryption(){
    
    //count of events. Need to change IV each time.
};
c::~symmetric_encryption(){};


void c::setKey(unsigned char* key, size_t length){
    
    // add logic to validate key and return false if fails.
    std::copy(key, key+length, c::key);
    
}

void c::generateKey(){
    
    c::prng.GenerateBlock( c::key, sizeof(c::key) );
   
}

void c::generateIV(){

    prng.GenerateBlock( c::iv, sizeof(c::iv) );    
    
}

bool c::encrypt(const string& plaintext, string& ciphertext, const int TAG_SIZE){
    
    //IV should be changed for each message.
    c::generateIV();


    try
    {
        cout << "plain text: " << plaintext << endl;

        GCM< AES >::Encryption e;
        e.SetKeyWithIV( key, sizeof(key), iv, sizeof(iv) );
        
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
    //also put in test for cast (check there is null termination)
    
    //ciphertext = string(reinterpret_cast<char *>(c::iv), AES::BLOCKSIZE) + ciphertext;


    return true;

}

bool c::decrypt(const string& ciphertext, string& plaintext, const int TAG_SIZE){
    
    try
    {
        GCM< AES >::Decryption d;
        d.SetKeyWithIV( key, sizeof(key), iv, sizeof(iv) );
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
    return true;

}








