#include "us/gov/crypto/symmetric_encryption.h"

#include <iostream>
#include <string>
#include "us/gov/crypto/ec.h"
#include "us/gov/crypto/base58.h" //and maybe this
#include <vector>
#include <secp256k1_ecdh.h>  //take this out after debugging.
#include <crypto++/aes.h>    //and this
#include <array>

using namespace std;
using namespace us::gov::crypto;
using CryptoPP::AES;

int main ( int argc, char *argv[] )
{
    if ( argc < 5 ) {
        cout<<"required arugments: <private key> <public key> <[encrypt|decrypt]> <message>" << endl;

        if(argc == 2)
            cout<<"provided private key: " << argv[1] << endl;
            
        if(argc == 3)
            cout<<"provided private key: " << argv[1] << "\n" << "provided public key: " << argv[2] << endl;
        if(argc == 4)
            cout<<"provided private key: " << argv[1] << "\n" << "provided public key: " << argv[2] << "\n" << "provided command: " << argv[3] << endl;
        
    }
    else {
        //cout<<"provided private key: " << argv[1] << "\n" << "provided public key: " << argv[2] << "\n" << "provided command: " << argv[3] << "\n" << "provided message: " << argv[4] << endl;
        ec::keys k;
        std::string argv1(argv[1]);
        k.priv=ec::keys::priv_t::from_b58(argv1);
        k.pub=ec::keys::pub_t::from_b58(argv[2]);
        cout<<"c++ private key: " << k.priv << endl;
        cout<<"c++ recieved public key: " << k.pub << endl;
        cout<<"private key hex:" << k.to_hex(k.priv)<< endl;
        cout << "pub key hex:" << k.pub.to_hex() << endl;
        vector<unsigned char> priv_decoded;
        b58::decode(argv1, priv_decoded);
        for (int i = 0; i < priv_decoded.size(); i++) {
            cout <<int(priv_decoded[i]) << "/";
            }
        unsigned char key_[AES::DEFAULT_KEYLENGTH]; //and this
        
        string command(argv[3]);

        string message_string(argv[4]);

        //vector<unsigned char> message(message_string.begin(),message_string.end());
       
        if(!ec::instance.generate_shared_key(key_, sizeof(key_), k.priv, k.pub)){
            cout << "couldn't make shared key" << endl;
        }
        else{
            //string shared_key = b58::encode(key_);
            //cout <<"shared key: " << endl;
            //for (int i = 0; i < AES::DEFAULT_KEYLENGTH; i++) {
            //cout << key_[i];
            //}
            //cout << endl;

            //cout << "shared key b58: " << b58::encode(key_, key_ + sizeof(key_) - 1);
            //cout << endl;
           
            //cout << hex << key_ << endl;

            //symmetric_encryption s_e(k.priv,k.pub);
           // if(command=="encrypt"){
            
                //vector<unsigned char> encrypted = s_e.encrypt(message);
                //string encrypted_string(encrypted.begin(), encrypted.end());
               // cout << "encrypted string : " << encrypted_string << endl;
           // }
        }
    }
}


//keys::priv_t <== taken from cmdline args
//keys::pub_t  <== taken from cmdline args
//command
//message  <== taken from cmdline args


//if comand==send {
   ////cypher message
   ////symmetric_encryption o(const keys::priv_t&, const keys::pub_t&);

  // prints encrypted payload to console
//}
//if (cmd==recv) {
  // prints decoded to stdout
//}




//}
