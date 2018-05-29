#include <iostream>
#include <string>
#include <cassert>
#include <us/gov/crypto/symmetric_encryption.h>

#include "crypto_test.h"

using namespace std;
typedef us::test::cryptotest t;
typedef us::gov::crypto c


bool t::test(const string& input, const string& expected ) {
	
	
	if( t:resolve_filename(input) != expected )  {  	 
		assert (false);
	 }
   return true;
}

bool testing_symmetric_encryption(){
	
	test_encrypt_decrypt()
	
}

bool debugging_symmetric_encryption(){
	c::symmetric_encryption s_e;

	string plaintext = "encrypt this";
	string ciphertext = "";
	s_e.generateKey();
	s_e.encrypt(plaintext,ciphertext);
	s_e.decrypt(ciphertext,plaintext);
	cout << plaintext << endl;
	cout << ciphertext << endl;
	
	return true;
}

bool test_encrypt_decrypt(string plaintext){
	c::symmetric_encryption s_e;
	string ciphertext;
	s_e.generateKey();
	s_e.encrypt(plaintext,ciphertext);
	string decodedtext;
	s_e.decrypt(ciphertext,decodedtext);
	assert(plaintext==decodedtext);
}

string get_encrypted_text(string plaintext)
	c::symmetric_encryption s_e;
	string ciphertext;
	s_e.generateKey();
	s_e.encrypt(plaintext,ciphertext);
	return ciphertext;

bool test decrypt()


//test encrypt+decrypt gives original result
//test ciphertest different to plaintext
//test decoded text different to ciphertext
//test with setting key and generating key.
//test that it won't run without key set.
//test that it doesn't decode successfully with the wrong key.
//test with short string and long string and empty string.


//test key generation spread?
//test that iv is different?
//test with other tag lengths?

//different strings

//different key gens

//





