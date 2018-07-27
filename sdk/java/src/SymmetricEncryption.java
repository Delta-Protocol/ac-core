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


import java.nio.charset.StandardCharsets;

public class SymmetricEncryption {

    private static final SecureRandom random = new SecureRandom();
    private static final int ivSize = 12;
    private static final int keySize = 16;

    private Cipher cipher;
    private byte[] key;
    private byte[] iv;

    public SymmetricEncryption(byte[] sharedKey) throws GeneralSecurityException {
        
        iv = new byte[ivSize];
        Security.addProvider(new BouncyCastleProvider());
        cipher = Cipher.getInstance("AES/GCM/NoPadding");
        if(sharedKey.length == keySize){
           key = sharedKey;
        } 
        else{
            throw new GeneralSecurityException("The key provided should be " + keySize + " bytes.");
        }
    }

    public int getKeySize(){
        return keySize;
    }

    public SymmetricEncryption(PrivateKey priv_a, PublicKey pub_b) throws GeneralSecurityException {
        
        this(EllipticCryptography.getInstance().generateSharedKey(priv_a,pub_b, keySize));
    }

    public byte[] encrypt(byte[] plaintext) throws GeneralSecurityException {
        
        random.nextBytes(iv);
        
        cipher.init(Cipher.ENCRYPT_MODE, new SecretKeySpec(key, "AES"), new IvParameterSpec(iv), random);
        return Arrays.concatenate(cipher.doFinal(plaintext), iv);
    }

    //Decrypt returns an empty byte array if the ciphertext is invalid. Invalid ciphertext would 
    //otherwise cause an exception as the algorithm is unable to authenticate the ciphertext.
    public byte[] decrypt(byte[] encrypted) {    
        
        byte[] emptyArray = new byte[0];
        try{
            int messageLength = encrypted.length - ivSize;
            if(messageLength<0){
                System.out.println("The ciphertext does not have sufficient length to contain the iv (initialisation vector) which should have been appended.");
                return emptyArray;
            }
            iv = Arrays.copyOfRange(encrypted, messageLength , encrypted.length);
            cipher.init(Cipher.DECRYPT_MODE, new SecretKeySpec(key, "AES"), new IvParameterSpec(iv), random);
            return cipher.doFinal(Arrays.copyOfRange(encrypted, 0, messageLength));
        }
        catch(GeneralSecurityException e){
            return emptyArray;
        }      
    }

}
