#include "crypto_test.h"
#include <iostream>
#include <cassert>
#include <us/gov/crypto/symmetric_encryption.h>



using namespace std;

using us::gov::crypto::symmetric_encryption;

bool test_symmetric_encryption(){
	
	keys keys_a = keys::generate();
	keys keys_b = keys::generate();
	keys keys_c = keys::generate();

	//test that encryption then decryption will retrieve original plaintext
	assert(test_encrypt_decrypt("encrypt this",								keys_a.priv,	keys_a.pub, keys_b.priv, keys_b.pub));
	assert(test_encrypt_decrypt("",											keys_a.priv,	keys_a.pub, keys_b.priv, keys_b.pub));
	assert(test_encrypt_decrypt(",./;'#[]-=123456DFJLSKDFJERUEIUR  \n",  	keys_a.priv,	keys_a.pub, keys_b.priv, keys_b.pub));
	assert(test_encrypt_decrypt("0",										keys_a.priv,	keys_a.pub, keys_b.priv, keys_b.pub));

	//test that message can't be decoded with the wrong key
	assert(!test_encrypt_decrypt("encrypt this",							keys_a.priv,	keys_a.pub, keys_c.priv, keys_b.pub));
}

bool test_encrypt_decrypt(string plaintext, keys::priv_t priv_a, keys::pub_t pub_a, keys::priv_t priv_b, keys::pub_t pub_b){
	
	string ciphertext = "";
	symmetric_encryption se_a(priv_a,pub_b);
	ciphertext = se_a.encrypt(plaintext);

	string decodedtext = "";
	symmetric_encryption se_b(priv_b,pub_a);
	decodedtext = se_b.decrypt(ciphertext);
	
	return plaintext.compare(decodedtext)==0;
}
//other useful tests??
//-test ciphertest different to plaintext
//-test decoded text different to ciphertext
//-test that iv is different?




