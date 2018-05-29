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

#include <vector.h>

using namespace us::gov::crypto;
using namespace std;

typedef us::gov::crypto::symmetric_encryption c;



string c::encrypt(const string& plaintext)
{
    //we need a new iv for each message that is encrypted with the same key.
    prng.GenerateBlock(&iv[0],iv.size());


    string ciphertext;
    try
    {
        cout << "plain text: " << plaintext << endl;

        GCM< AES>::Encryption e;
        e.SetKeyWithIV( &key[0], key.size(), &iv[0], iv.size() );
        
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
    
    return string(reinterpret_cast<char *>(c::iv), iv.size() + ciphertext;
    

}

string splitIVCiphertext(string ivCiphertext)
{
    iv.clear();
    //iv.push_back(ivCiphertext.)
    
    return "";
}

string c::decrypt(const string& ivCiphertext){
    
    string ciphertext = splitIVCiphertext(ivCiphertext);
    string plaintext;
    try
    {
        GCM< AES >::Decryption d;
        d.SetKeyWithIV( &key[0], key.size(), &iv[0], iv.size() );
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








