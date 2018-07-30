
#include "us/gov/crypto/symmetric_encryption.h"

#include <iostream>
#include <string>
#include "us/gov/crypto/ec.h"
#include "us/gov/crypto/base58.h" 
#include <vector>
#include <array>

using namespace std;
using namespace us::gov::crypto;
using CryptoPP::AES;

int main ( int argc, char *argv[] )
{
    if ( argc < 4 ) {
        cout<<"required arugments: <key> <[sign|verify]> <message> <hash (optional)>" << endl;
        if(argc == 2)
            cout<<"provided key: " << argv[1] << endl;
        if(argc == 3)
            cout<<"provided key: " << argv[1] << "\n" << "provided command: " << argv[2] << endl;
    }
    else {
        
        ec::keys k;
        
        string command(argv[2]);

        symmetric_encryption s_e(k.priv, k.pub);
          
            if(command=="sign"){
                k.priv=ec::keys::priv_t::from_b58(argv[1]);
                string message_string(argv[4]);
                vector<unsigned char> message(message_string.begin(),message_string.end());

                vector<unsigned char> signed = ec.sign(message);
                cout << b58::encode(encrypted) << endl;
            }
            if(command=="verify"){
                k.pub=ec::keys::pub_t::from_b58(argv[2]);
                vector<unsigned char> message;
                b58::decode(argv[4],message);
                vector<unsigned char> decrypted = s_e.decrypt(message);
                string decrypted_string(decrypted.begin(), decrypted.end());
                cout << decrypted_string << endl;
            }
        
    }
}

