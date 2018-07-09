#include <us/gov/crypto/symmetric_encryption.h>

#include <iostream>
#include <string>
#include <us/gov/crypto/ec.h>

using namespace std;
using namespace us::gov::crypto;

int main ( int argc, char *argv[] )
{
    if ( argc < 4 ) {
        cout<<"required arugments: <private key> <wallet public key> <message>" << endl;

        if(argc = 2)
            cout<<"provided private key: " << argv[1] << endl;
            cout<<"no public key or message provided" << endl;
        if(argc = 3)
            cout<<"provided private key: " << argv[1] << "\n" << "provided public key: " << argv[2] << endl;
            cout<<"no message provided" << endl;
    }
    else {
        keys::priv_t priv;
        priv.from_b58(argv[1]);
        
        cout << priv.to_b58() << endl;

        keys::pub_t pub;
        pub.from_b58(argv[2]);
        
        cout << pub.to_b58() << endl; 

        string message(argv[3]);
    
        cout << message << endl;

        symmetric_encryption s_e(priv,pub)
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
