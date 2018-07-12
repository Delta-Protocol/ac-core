package us.wallet;

import java.security.SecureRandom;
import java.security.Security;
import javax.crypto.Cipher;
import javax.crypto.SecretKey;
import javax.crypto.spec.IvParameterSpec;
import org.spongycastle.util.Arrays;
import org.spongycastle.math.ec.ECPoint;
import org.spongycastle.jce.provider.BouncyCastleProvider;
import javax.crypto.spec.SecretKeySpec;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.GeneralSecurityException;

public class SymmetricEncryption {

    private static final SecureRandom random = new SecureRandom();
    private static final int iv_size = 16;
    private Cipher cipher;
    private byte[] key;
    private byte[] iv;

    public SymmetricEncryption(PrivateKey priv_a, PublicKey pub_b) throws GeneralSecurityException {
        Security.addProvider(new BouncyCastleProvider());
        cipher = Cipher.getInstance("AES/GCM/NoPadding");
        iv = new byte[iv_size];
        key = EllipticCryptography.getInstance().generateSharedKey(priv_a,pub_b);
        /*for (int i = 0; i < key.length; i++) {
             
            System.out.println("key: " + key[i]); 
        }*/
    }

    public byte[] encrypt(byte[] plaintext) throws GeneralSecurityException {
        random.nextBytes(iv);
        System.out.println("iv on encrypt: " + iv);
        /*for (int i = 0; i < iv.length; i++) {
             
            System.out.println("iv: " + iv[i]); 
        }*/
        
        cipher.init(Cipher.ENCRYPT_MODE, new SecretKeySpec(key, "AES"), new IvParameterSpec(iv), random);
        return Arrays.concatenate(cipher.doFinal(plaintext), iv);
    }

    public byte[] decrypt(byte[] encrypted) throws GeneralSecurityException {   
    
        try{
            iv = Arrays.copyOfRange(encrypted, encrypted.length - iv_size , encrypted.length - 1);
            System.out.println("iv on decrypt: " + iv);
            cipher.init(Cipher.DECRYPT_MODE, new SecretKeySpec(key, "AES"), new IvParameterSpec(iv), random);
            return cipher.doFinal(Arrays.copyOfRange(encrypted, 0, encrypted.length - iv_size));
        }
        catch(Exception e){
            return null;
        }
        
    }

}
