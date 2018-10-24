#include "symmetric_encryption.h"

#include <iterator>

#include <secp256k1_ecdh.h>

#include <crypto++/cryptlib.h>
#include <crypto++/filters.h>
#include <crypto++/gcm.h>
#include <crypto++/aes.h>

using CryptoPP::GCM;
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

symmetric_encryption::symmetric_encryption(const ec::keys::priv_t& priv_key_a, 
                                           const ec::keys::pub_t& pub_key_b) {
    if (!ec::get_instance().generate_shared_key(m_key_, sizeof(m_key_), priv_key_a, pub_key_b)) {
        throw "Could not initialize encryption";
    }
}

symmetric_encryption::symmetric_encryption(const vector<unsigned char>& shared_key) {
    if(shared_key.size()==m_key_size){
        for(size_t i = 0; i < m_key_size; i++){
            m_key_[i] = shared_key[i];
        }
    }
    else{
        throw "Could not initialize encryption. The key provided should be " + to_string(m_key_size) + " bytes.";
    } 
}

const vector<unsigned char> 
symmetric_encryption::encrypt(const vector<unsigned char>& plaintext) {  
    vector<unsigned char> ciphertext(m_iv_size + plaintext.size() + AES::BLOCKSIZE);
    try{
        m_prng_.GenerateBlock(m_iv_, m_iv_size);

        GCM<AES>::Encryption enc;
        enc.SetKeyWithIV(m_key_, m_key_size, m_iv_, m_iv_size);

        ArraySink cs(&ciphertext[0], ciphertext.size());

        ArraySource(plaintext.data(), plaintext.size(), true, 
                    new AuthenticatedEncryptionFilter(enc, new Redirector(cs),false, m_tag_size));

        ciphertext.resize(cs.TotalPutLength());
        ciphertext.insert(ciphertext.end(), begin(m_iv_), end(m_iv_));
    }
    catch( CryptoPP::InvalidArgument& e )
    {
        cerr << "Caught InvalidArgument...\n" << e.what() << "\n" << endl;
        return vector<unsigned char>();
    }
    return move(ciphertext);

}

void symmetric_encryption::set_iv_from_ciphertext(const vector<unsigned char>& ciphertext) {
    size_t iv_start = ciphertext.size() - m_iv_size;
    for(size_t i = 0; i < m_iv_size; i++){
        m_iv_[i] = ciphertext[i+iv_start];
    }
}

const vector<unsigned char> symmetric_encryption::decrypt(const vector<unsigned char>& ciphertext) {
    if(ciphertext.size()<m_iv_size){
        return vector<unsigned char>();
    }
    vector<unsigned char> decryptedtext(ciphertext.size());
    set_iv_from_ciphertext(ciphertext);

    try{
        GCM< AES >::Decryption d;
        d.SetKeyWithIV( m_key_, m_key_size, m_iv_, m_iv_size );

        ArraySink sink(decryptedtext.data(), decryptedtext.size());
        AuthenticatedDecryptionFilter filter(d, new Redirector(sink),
                AuthenticatedDecryptionFilter::DEFAULT_FLAGS, m_tag_size);
        ArraySource(ciphertext.data(), ciphertext.size() - m_iv_size, 
                                       true, new Redirector(filter));

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
