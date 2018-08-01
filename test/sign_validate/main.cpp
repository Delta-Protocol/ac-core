
#include "us/gov/crypto/symmetric_encryption.h"

#include <iostream>
#include <string>
#include "us/gov/crypto/ec.h"
#include "us/gov/crypto/base58.h" 
#include <vector>
#include <array>

using namespace std;
using namespace us::gov::crypto;

int main ( int argc, char *argv[] )
{
    if ( argc < 4 ) {
        cout<<"arugments: <key> <[sign|verify]> <message> <hash (optional)>" << endl;
        if(argc == 2)
            cout<<"provided key: " << argv[1] << endl;
        if(argc == 3)
            cout<<"provided key: " << argv[1] << "\n" << "provided command: " << argv[2] << endl;
    }
    else {
        
        ec::keys k;
        
        string command(argv[2]);
        string message(argv[3]);
          
            if(command=="sign"){
                
                k.priv=ec::keys::priv_t::from_b58(argv[1]);
                
                string signature = ec::instance.sign_encode(k.priv, message);
                
                cout << signature << endl;
                
            }
            if(command=="verify"){
                k.pub=ec::keys::pub_t::from_b58(argv[1]);
                string signature = argv[4];
                bool verified = ec::instance.verify_not_normalized(k.pub, message, signature);
                cout << verified << endl;
            }
        
    }
}


