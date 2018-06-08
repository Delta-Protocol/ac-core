#include "crypto_test.h"
#include <iostream>
#include <cassert>
#include <us/gov/crypto/symmetric_encryption.h>



using namespace std;

using us::gov::crypto::symmetric_encryption;

bool test_symmetric_encryption(){
	
	test_encrypt_multiple("The ciphertext should be different although the plaintext is the same");
	test_encrypt_decrypt("encrypt this"																														);
	//test that same plaintext is encrypted to different ciphertexts.
	assert(test_encrypt_multiple("The ciphertext should be different although the plaintext is the same"));
	
	//test that encryption then decryption will retrieve original plaintext
	test_encrypt_decrypt("encrypt this"																														);
	assert(test_encrypt_decrypt(""																																	));
	assert(test_encrypt_decrypt(",./;'#[]-=123456DFJLSKDFJERUEIUR  \n"  																							));
	assert(test_encrypt_decrypt("0"																																	));
	assert(test_encrypt_decrypt("𝕋𝕙𝕖 𝕢𝕦𝕚𝕔𝕜 𝕓𝕣𝕠𝕨𝕟 𝕗𝕠𝕩 𝕛𝕦𝕞𝕡𝕤 𝕠𝕧𝕖𝕣 𝕥𝕙𝕖 𝕝𝕒𝕫𝕪 𝕕𝕠𝕘"																					 ));
	assert(test_encrypt_decrypt("パーティーへ行かないか"																												 ));
	assert(test_encrypt_decrypt("˙ɐnbᴉlɐ ɐuƃɐɯ ǝɹolop ʇǝ ǝɹoqɐl ʇn ʇunpᴉpᴉɔuᴉ ɹodɯǝʇ poɯsnᴉǝ op pǝs 'ʇᴉlǝ ƃuᴉɔsᴉdᴉpɐ ɹnʇǝʇɔǝsuoɔ 'ʇǝɯɐ ʇᴉs ɹolop ɯnsdᴉ ɯǝɹo˥00˙Ɩ$-" ));
	assert(test_encrypt_decrypt("null"																																));
	assert(test_encrypt_decrypt("-$1.00"																															));
	
	//test that message can't be decoded with the wrong key

	keys a = keys::generate();
	keys b = keys::generate();
	keys c = keys::generate();

	assert(!test_encrypt_decrypt_keys("encrypt this", a.priv,	a.pub, c.priv, b.pub));

	return true;
}

bool test_encrypt_decrypt(string plaintext){
	
	keys a = keys::generate();
	keys b = keys::generate();
   
	bool changethis =  test_encrypt_decrypt_keys(plaintext,a.priv,a.pub,b.priv,b.pub);
	return changethis;
}

bool test_encrypt_decrypt_keys(string plaintext,const keys::priv_t& priv_a, const keys::pub_t& pub_a, const keys::priv_t& priv_b, const keys::pub_t& pub_b){

	symmetric_encryption se_a(priv_a,pub_b);
	string ciphertext = se_a.encrypt(plaintext);

	symmetric_encryption se_b(priv_b,pub_a);
	string decodedtext = se_b.decrypt(ciphertext);
	
	return plaintext.compare(decodedtext)==0;
}


bool test_encrypt_multiple(string plaintext){
	
	keys a = keys::generate();
	keys b = keys::generate();

	symmetric_encryption se_a(a.priv,b.pub);
	string ciphertext_1 = se_a.encrypt(plaintext);
	string ciphertext_2 = se_a.encrypt(plaintext);

	return ciphertext_1.compare(ciphertext_2)!=0;
	
}

bool testdecrypt(string ciphertext_1){

}

bool test_decrypt_multiple(string plaintext){

	keys a = keys::generate();
	keys b = keys::generate();

	symmetric_encryption se_a(a.priv,b.pub);
	string ciphertext = se_a.encrypt(plaintext);

	symmetric_encryption se_b(b.priv,a.pub);
	string decodedtext1 = se_b.decrypt(ciphertext);
	string decodedtext2 = se_b.decrypt(ciphertext);
	
	return decodedtext1.compare(decodedtext2)==0;
	
}
//other useful tests??
//-test ciphertest different to plaintext
//-test decoded text different to ciphertext





