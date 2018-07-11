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
    private static final int iv_size = 12;
    private Cipher cipher;
    private byte[] key;
    private byte[] iv;

    public SymmetricEncryption(PrivateKey priv_a, PublicKey pub_b) throws GeneralSecurityException {
        Security.insertProviderAt(new BouncyCastleProvider(), 1);
        cipher = Cipher.getInstance("AES/GCM/NoPadding", "BC");
        iv = new byte[iv_size];
        key = EllipticCryptography.getInstance().generateSharedKey(priv_a,pub_b);
    }

    public byte[] encrypt(byte[] plaintext) throws GeneralSecurityException {
        random.nextBytes(iv);
        cipher.init(Cipher.ENCRYPT_MODE, new SecretKeySpec(key, "AES"), new IvParameterSpec(iv), random);
        return Arrays.concatenate(cipher.doFinal(plaintext), iv);
    }

    public byte[] decrypt(byte[] encrypted) throws GeneralSecurityException {   
    
        try{
            iv = Arrays.copyOfRange(encrypted, encrypted.length - iv_size, encrypted.length);
            cipher.init(Cipher.DECRYPT_MODE, new SecretKeySpec(key, "AES"), new IvParameterSpec(iv), random);
            return cipher.doFinal(Arrays.copyOfRange(encrypted, 0, encrypted.length - iv_size));
        }
        catch(Exception e){
            return null;
        }
        
    }

}
