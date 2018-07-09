package us.test;

import us.wallet.*;
import java.security.PrivateKey;
import java.security.PublicKey;

import org.spongycastle.crypto.AsymmetricCipherKeyPair;


public class SymmetricEncryptionTests{

    EllipticCryptography ec = EllipticCryptography.getInstance();
        AsymmetricCipherKeyPair a = ec.generateKeyPair();
        AsymmetricCipherKeyPair b = ec.generateKeyPair();
        AsymmetricCipherKeyPair c = ec.generateKeyPair();

    public boolean test_symmetric_encryption(){
    
       
        
        //test that encryption then decryption will retrieve original plaintext.
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

        //test that ciphertext is decrypted to the same plaintext.
        assert(test_decrypt_multiple("The plaintext should be the same each time we decrypt"));

        //test that message can't be decoded with the wrong key.
        assert(!test_encrypt_decrypt_keys("encrypt this", a.getPrivate(),	a.getPublic(), c.getPrivate(), b.getPublic()));

        //decrypting invalid ciphertext should return an empty vector.
        assert(test_decrypt_nulls(""));
        assert(test_decrypt_nulls("this string hasn't been encrypted so decryption will fail"));
        
        return true;
    }

    public boolean test_encrypt_decrypt(String plaintext_string){
        
        AsymmetricCipherKeyPair a = ec.generateKeyPair();
        AsymmetricCipherKeyPair b = ec.generateKeyPair();
        
        
        return test_encrypt_decrypt_keys(plaintext_string,a.getPrivate(),a.getPublic(),b.getPrivate(),b.getPublic());
    }

    public boolean test_encrypt_decrypt_keys(String plaintext_string, PublicKey pubA, PrivateKey privA, PublicKey pubB, PrivateKey privB){

        byte[] plaintext = plaintext_string.getBytes();
        
        SymmetricEncryption se_a= new SymmetricEncryption(privA,pubB);
        byte[] ciphertext = se_a.encrypt(plaintext);

        SymmetricEncryption se_b= new SymmetricEncryption(privB,pubA);
        byte[] decodedtext = se_b.decrypt(ciphertext);
        
        return plaintext==decodedtext;
    }

    public boolean test_encrypt_multiple(String plaintext_string){
        
        AsymmetricCipherKeyPair a = ec.generateKeyPair();
        AsymmetricCipherKeyPair b = ec.generateKeyPair();

        byte[] plaintext = plaintext_string.getBytes();
        
        SymmetricEncryption se_a = new SymmetricEncryption(a.getPrivate(),b.getPublic());
        byte[] ciphertext_1 = se_a.encrypt(plaintext);
        byte[] ciphertext_2 = se_a.encrypt(plaintext);

        return ciphertext_1!=ciphertext_2;
    }

    public boolean test_decrypt_nulls(String ciphertext_string){
        
        AsymmetricCipherKeyPair a = ec.generateKeyPair();
        AsymmetricCipherKeyPair b = ec.generateKeyPair();

        byte[] ciphertext = ciphertext_string.getBytes();
        SymmetricEncryption se = new SymmetricEncryption(a.getPrivate(),b.getPublic());
        byte[] decryptedtext = se.decrypt(ciphertext);
        return decryptedtext.length>0;
    }

    public boolean test_decrypt_multiple(String plaintext_string){

        AsymmetricCipherKeyPair a = ec.generateKeyPair();
        AsymmetricCipherKeyPair b = ec.generateKeyPair();

        byte[] plaintext = plaintext_string.getBytes();
        
        SymmetricEncryption se_a = new SymmetricEncryption(a.getPrivate(),b.getPublic());
        byte[] ciphertext = se_a.encrypt(plaintext);

        SymmetricEncryption se_b = new SymmetricEncryption(b.getPrivate(),a.getPublic());
        byte[] decryptedtext1 = se_b.decrypt(plaintext);
        byte[] decryptedtext2 = se_b.decrypt(plaintext);
        
        return decryptedtext1==decryptedtext2;
        
    }

    
}