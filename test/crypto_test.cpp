#include "crypto_test.h"
#include <iostream>
#include <cassert>
#include <us/gov/crypto/SymmetricEncryption.h>



using namespace std;

using us::gov::crypto::SymmetricEncryption;

bool TestSymmetricEncryption(){
	
	keys keysA = keys::generate();
	keys keysB = keys::generate();
	keys keysC = keys::generate();

	//test that encryption then decryption will retrieve original plaintext
	assert(TestEncryptDecrypt("encrypt this",							keysA.priv,	keysA.pub, keysB.priv, keysB.pub));
	assert(TestEncryptDecrypt("",										keysA.priv,	keysA.pub, keysB.priv, keysB.pub));
	assert(TestEncryptDecrypt(",./;'#[]-=123456DFJLSKDFJERUEIUR  \n",  	keysA.priv,	keysA.pub, keysB.priv, keysB.pub));
	assert(TestEncryptDecrypt("0",										keysA.priv,	keysA.pub, keysB.priv, keysB.pub));

	//test that message can't be decoded with the wrong key
	assert(!TestEncryptDecrypt("encrypt this",							keysA.priv,	keysA.pub, keysC.priv, keysB.pub));
}

bool TestEncryptDecrypt(string plaintext, keys::priv_t privA, keys::pub_t pubA, keys::priv_t privB, keys::pub_t pubB){
	
	string ciphertext = "";
	SymmetricEncryption seA(privA,pubB);
	ciphertext = seA.Encrypt(plaintext);

	string decodedText = "";
	SymmetricEncryption seB(privB,pubA);
	decodedText = seB.Decrypt(ciphertext);
	
	return plaintext.compare(decodedText)==0;
}
//useful tests??
//-test ciphertest different to plaintext
//-test decoded text different to ciphertext

//test that iv is different?




