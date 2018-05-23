#include <iostream>
#include <string>
#include <cassert>
#include <us/gov/crypto/symmetric_encryption.h>

#include "crypto_test.h"

using namespace std;
typedef us::test::cryptotest t;


bool test(const string& input, const string& expected ) {
	
	//if( us::gov::crypto::daemon::resolve_filename(input) != expected )  {  	 
		//assert (false);
	 //}
  // return true;
//}

bool testing_symmetric_encryption(){

	
}

bool debugging_symmetric_encryption(){
	us::gov::crypto::symmetric_encryption s_e;

	string plaintext = "encrypt this";
	string ciphertext = "";
	s_e.generateKey();
	s_e.encrypt(plaintext,ciphertext);
	s_e.decrypt(ciphertext,plaintext);
	cout << plaintext << endl;
	cout << ciphertext << endl;
	
	return true;
}

bool test_encrypt(){

}

