#include "us/gov/crypto/symmetric_encryption.h"

#include <iostream>
#include <string>
#include "us/gov/crypto/ec.h"
#include <vector>

using namespace std;
using namespace us::gov::crypto;

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
        
        string command(argv[3]);

        string message_string(argv[4]);

        vector<unsigned char> message(message_string.begin(),message_string.end());
        
        symmetric_encryption s_e(k.priv,k.pub);
        if(command=="encrypt"){
        
           vector<unsigned char> encrypted = s_e.encrypt(message);
           string encrypted_string(encrypted.begin(), encrypted.end());
           cout << encrypted_string << endl;
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
