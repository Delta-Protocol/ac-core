package us.test;

import us.wallet.*;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.KeyPair;
import java.security.GeneralSecurityException;



public class SymmetricEncryptionTests{

    
        private static EllipticCryptography ec;
        
        KeyPair a;
        KeyPair b;
        KeyPair c;

    public boolean test_symmetric_encryption()throws GeneralSecurityException{
    
        try{
            
            ec = EllipticCryptography.getInstance();
            a = ec.generateKeyPair();
            b = ec.generateKeyPair();
            b = ec.generateKeyPair();

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
        }
        catch(GeneralSecurityException e){
            throw new GeneralSecurityException("encryption test failed on: " + e);
        }
        return true;
    }

    public boolean test_encrypt_decrypt(String plaintext_string) throws GeneralSecurityException{
        
        KeyPair a = ec.generateKeyPair();
        KeyPair b = ec.generateKeyPair();

        PrivateKey priv_a = ec.privateKeyFromKeyPair(a);
        PublicKey pub_a = ec.publicKeyFromKeyPair(a);
        PrivateKey priv_b = ec.privateKeyFromKeyPair(b);
        PublicKey pub_b = ec.publicKeyFromKeyPair(b);

        return test_encrypt_decrypt_keys(plaintext_string, priv_a, pub_a, priv_b, pub_b);
    }

    public boolean test_encrypt_decrypt_keys(String plaintext_string, PrivateKey priv_a, PublicKey pub_a, PrivateKey priv_b, PublicKey pub_b) throws GeneralSecurityException{

        byte[] plaintext = plaintext_string.getBytes();
        
        SymmetricEncryption se_a= new SymmetricEncryption(priv_a,pub_b);
        byte[] ciphertext = se_a.encrypt(plaintext);

        SymmetricEncryption se_b= new SymmetricEncryption(priv_b,pub_a);
        byte[] decodedtext = se_b.decrypt(ciphertext);
        
        return plaintext==decodedtext;
    }

    public boolean test_encrypt_multiple(String plaintext_string)throws GeneralSecurityException{
        
        KeyPair a = ec.generateKeyPair();
        KeyPair b = ec.generateKeyPair();

        PrivateKey priv_a = ec.privateKeyFromKeyPair(a);
        PublicKey pub_b = ec.publicKeyFromKeyPair(b);

        byte[] plaintext = plaintext_string.getBytes();
        
        SymmetricEncryption se_a = new SymmetricEncryption(priv_a,pub_b);
        byte[] ciphertext_1 = se_a.encrypt(plaintext);
        byte[] ciphertext_2 = se_a.encrypt(plaintext);

        return ciphertext_1!=ciphertext_2;
    }

    public boolean test_decrypt_nulls(String ciphertext_string) throws GeneralSecurityException{
        
        KeyPair a = ec.generateKeyPair();
        KeyPair b = ec.generateKeyPair();

        PrivateKey priv_a = ec.privateKeyFromKeyPair(a);
        PublicKey pub_b = ec.publicKeyFromKeyPair(b);

        byte[] ciphertext = ciphertext_string.getBytes();
        SymmetricEncryption se = new SymmetricEncryption(priv_a,pub_b);
        byte[] decryptedtext = se.decrypt(ciphertext);
        return decryptedtext.length>0;
    }

    public boolean test_decrypt_multiple(String plaintext_string) throws GeneralSecurityException{

        KeyPair a = ec.generateKeyPair();
        KeyPair b = ec.generateKeyPair();

        PrivateKey priv_a = ec.privateKeyFromKeyPair(a);
        PublicKey pub_a = ec.publicKeyFromKeyPair(a);
        PrivateKey priv_b = ec.privateKeyFromKeyPair(b);
        PublicKey pub_b = ec.publicKeyFromKeyPair(b);

        byte[] plaintext = plaintext_string.getBytes();
        
        SymmetricEncryption se_a = new SymmetricEncryption(priv_a,pub_b);
        byte[] ciphertext = se_a.encrypt(plaintext);

        SymmetricEncryption se_b = new SymmetricEncryption(priv_b,pub_a);
        byte[] decryptedtext1 = se_b.decrypt(plaintext);
        byte[] decryptedtext2 = se_b.decrypt(plaintext);
        
        return decryptedtext1==decryptedtext2;
        
    }

    
}