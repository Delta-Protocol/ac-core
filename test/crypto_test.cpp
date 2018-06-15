#include "crypto_test.h"
#include <iostream>
#include <cassert>
#include <vector>
#include <us/gov/crypto/symmetric_encryption.h>

using namespace std;

using us::gov::crypto::symmetric_encryption;

bool test_symmetric_encryption(){
	
	keys a = keys::generate();
	keys b = keys::generate();
	keys c = keys::generate();
	
	//test that encryption then decryption will retrieve original plaintext
	test_encrypt_decrypt("encrypt this"																																);
	assert(test_encrypt_decrypt(""																																	));
	assert(test_encrypt_decrypt(",./;'#[]-=123456DFJLSKDFJERUEIUR  \n rtr"  																						));
	assert(test_encrypt_decrypt("0"																																	));
	assert(test_encrypt_decrypt("𝕋𝕙𝕖 𝕢𝕦𝕚𝕔𝕜 𝕓𝕣𝕠𝕨𝕟 𝕗𝕠𝕩 𝕛𝕦𝕞𝕡𝕤 𝕠𝕧𝕖𝕣 𝕥𝕙𝕖 𝕝𝕒𝕫𝕪 𝕕𝕠𝕘"																					 ));
	assert(test_encrypt_decrypt("パーティーへ行かないか"																												 ));
	assert(test_encrypt_decrypt("˙ɐnbᴉlɐ ɐuƃɐɯ ǝɹolop ʇǝ ǝɹoqɐl ʇn ʇunpᴉpᴉɔuᴉ ɹodɯǝʇ poɯsnᴉǝ op pǝs 'ʇᴉlǝ ƃuᴉɔsᴉdᴉpɐ ɹnʇǝʇɔǝsuoɔ 'ʇǝɯɐ ʇᴉs ɹolop ɯnsdᴉ ɯǝɹo˥00˙Ɩ$-" ));
	assert(test_encrypt_decrypt("null"																																));
	assert(test_encrypt_decrypt("-$1.00"																															));
	
	//test that same plaintext is encrypted to different ciphertexts.
	assert(test_encrypt_multiple("The ciphertext should be different although the plaintext is the same"));

	//test that message can't be decoded with the wrong key
	assert(!test_encrypt_decrypt_keys("encrypt this", a.priv,	a.pub, c.priv, b.pub));

	return true;
}

bool test_encrypt_decrypt(string plaintext_string){
	
	keys a = keys::generate();
	keys b = keys::generate();
	
	return test_encrypt_decrypt_keys(plaintext_string,a.priv,a.pub,b.priv,b.pub);
}

bool test_encrypt_decrypt_keys(string plaintext_string,const keys::priv_t& priv_a, const keys::pub_t& pub_a, const keys::priv_t& priv_b, const keys::pub_t& pub_b){

	vector<unsigned char> plaintext(plaintext_string.begin(),plaintext_string.end());
	
	symmetric_encryption se_a(priv_a,pub_b);
	const vector<unsigned char> ciphertext = se_a.encrypt(plaintext);

	symmetric_encryption se_b(priv_b,pub_a);
	const vector<unsigned char> decodedtext = se_b.decrypt(ciphertext);
	
	return plaintext==decodedtext;
}

bool test_encrypt_multiple(string plaintext_string){
	
	keys a = keys::generate();
	keys b = keys::generate();

	vector<unsigned char> plaintext(plaintext_string.begin(),plaintext_string.end());
	
	symmetric_encryption se_a(a.priv,b.pub);
	const vector<unsigned char> ciphertext_1 = se_a.encrypt(plaintext);
	const vector<unsigned char> ciphertext_2 = se_a.encrypt(plaintext);

	return ciphertext_1!=ciphertext_2;
}

bool testdecrypt(string ciphertext, string expected_result){
	
	keys a = keys::generate();
	keys b = keys::generate();

	vector<unsigned char> plaintext(plaintext_string.begin(),plaintext_string.end());


}

/*bool test_decrypt_multiple(string plaintext){

	keys a = keys::generate();
	keys b = keys::generate();

	symmetric_encryption se_a(a.priv,b.pub);
	string ciphertext = se_a.encrypt(plaintext);

	symmetric_encryption se_b(b.priv,a.pub);
	string decodedtext1 = se_b.decrypt(ciphertext);
	string decodedtext2 = se_b.decrypt(ciphertext);
	
	return decodedtext1.compare(decodedtext2)==0;
	
}*/
//other useful tests??
//-test ciphertest different to plaintext
//-test decoded text different to ciphertext





