package us.test;

import us.wallet.*;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.KeyPair;
import java.security.GeneralSecurityException;
import java.util.Arrays;



public class SymmetricEncryptionTests{

    
        private static EllipticCryptography ec;
        
        KeyPair a;
        KeyPair b;
        KeyPair c;

    public boolean testSymmetricEncryption() throws Exception{
    
        try{
            
            ec = EllipticCryptography.getInstance();
            a = ec.generateKeyPair();
            b = ec.generateKeyPair();
            c = ec.generateKeyPair();

            PrivateKey priv_a = a.getPrivate();
            PublicKey pub_a = a.getPublic();
            PrivateKey priv_b = b.getPrivate();
            PublicKey pub_b = b.getPublic();

            PrivateKey priv_c = c.getPrivate();
            PublicKey pub_c = c.getPublic();

            //test that encryption then decryption will retrieve original plaintext.
            assert(test_encrypt_decrypt("encrypt this"																														));
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
            assert(!test_encrypt_decrypt_keys("test that message can't be decoded with the wrong key", priv_a, pub_b, priv_c, pub_a));

            //decrypting invalid ciphertext should return an empty vector. 
            assert(test_decrypt_nulls(""));
            assert(test_decrypt_nulls("this string hasn't been encrypted so decryption will fail"));
            
        }
        catch(Exception e){
            throw new Exception("encryption test failed on: " + e);
        }
        return true;
    }

    public boolean test_encrypt_decrypt(String plaintext_string) throws GeneralSecurityException{
        
        KeyPair a = ec.generateKeyPair();
        KeyPair b = ec.generateKeyPair();

        PrivateKey priv_a = a.getPrivate();
        PublicKey pub_a = a.getPublic();
        PrivateKey priv_b = b.getPrivate();
        PublicKey pub_b = b.getPublic();
        return test_encrypt_decrypt_keys(plaintext_string, priv_a, pub_b, priv_b, pub_a);
    }

    public boolean test_encrypt_decrypt_keys(String plaintext_string, PrivateKey priv_a, PublicKey pub_b, PrivateKey priv_b, PublicKey pub_a) throws GeneralSecurityException{
        
        byte[] plaintext = plaintext_string.getBytes();
        
        SymmetricEncryption se_a= new SymmetricEncryption(priv_a,pub_b);
        byte[] ciphertext = se_a.encrypt(plaintext);
       
        SymmetricEncryption se_b= new SymmetricEncryption(priv_b,pub_a);
        byte[] decryptedtext = se_b.decrypt(ciphertext);
      
        return Arrays.equals(plaintext,decryptedtext);
    }

    public boolean test_encrypt_multiple(String plaintext_string)throws GeneralSecurityException{
        
        KeyPair a = ec.generateKeyPair();
        KeyPair b = ec.generateKeyPair();

        PrivateKey priv_a = a.getPrivate();
        PublicKey pub_a = a.getPublic();
        PrivateKey priv_b = b.getPrivate();
        PublicKey pub_b = b.getPublic();
        
        byte[] plaintext = plaintext_string.getBytes();
        
        SymmetricEncryption se_a = new SymmetricEncryption(priv_a,pub_b);
        byte[] ciphertext_1 = se_a.encrypt(plaintext);
        byte[] ciphertext_2 = se_a.encrypt(plaintext);

        return !Arrays.equals(ciphertext_1,ciphertext_2);
    }

    public boolean test_decrypt_nulls(String ciphertext_string) throws GeneralSecurityException{
        
        KeyPair a = ec.generateKeyPair();
        KeyPair b = ec.generateKeyPair();

        PrivateKey priv_a = a.getPrivate();
        PublicKey pub_a = a.getPublic();
        PrivateKey priv_b = b.getPrivate();
        PublicKey pub_b = b.getPublic();

        byte[] ciphertext = ciphertext_string.getBytes();
        SymmetricEncryption se = new SymmetricEncryption(priv_a,pub_b);
        byte[] decryptedtext = se.decrypt(ciphertext);
        return Arrays.equals(decryptedtext,new byte[0]);
    }

    public boolean test_decrypt_multiple(String plaintext_string) throws GeneralSecurityException{

        KeyPair a = ec.generateKeyPair();
        KeyPair b = ec.generateKeyPair();

        PrivateKey priv_a = a.getPrivate();
        PublicKey pub_a = a.getPublic();
        PrivateKey priv_b = b.getPrivate();
        PublicKey pub_b = b.getPublic();

        byte[] plaintext = plaintext_string.getBytes();
        
        SymmetricEncryption se_a = new SymmetricEncryption(priv_a,pub_b);
        byte[] ciphertext = se_a.encrypt(plaintext);

        SymmetricEncryption se_b = new SymmetricEncryption(priv_b,pub_a);
        byte[] decryptedtext1 = se_b.decrypt(ciphertext);
        byte[] decryptedtext2 = se_b.decrypt(ciphertext);
     
        return Arrays.equals(decryptedtext1,decryptedtext2);
    }

    
}