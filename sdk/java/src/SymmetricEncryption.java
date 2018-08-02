package us.wallet;

import org.spongycastle.jce.provider.BouncyCastleProvider;
import org.spongycastle.util.Arrays;
import javax.crypto.Cipher;
import javax.crypto.spec.GCMParameterSpec;
import javax.crypto.spec.SecretKeySpec;
import java.security.spec.AlgorithmParameterSpec;
import java.security.SecureRandom;
import java.security.Security;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.GeneralSecurityException;

public class SymmetricEncryption {

    private static final SecureRandom random = new SecureRandom();
    
    private static final int ivSize = 12;                   //all sizes in bytes
    private static final int keySize = 16;
    private static final int tagSize = 16;

    private Cipher cipher;
    private SecretKeySpec keySpec;
    private byte[] iv;

    public SymmetricEncryption(byte[] sharedKey) throws GeneralSecurityException {
        
        iv = new byte[ivSize];
        Security.addProvider(new BouncyCastleProvider());
        cipher = Cipher.getInstance("AES/GCM/NoPadding");
        if(sharedKey.length == keySize){
            keySpec = new SecretKeySpec(sharedKey, "AES");
        } 
        else{
            throw new GeneralSecurityException("The key provided should be " + keySize + " bytes.");
        }
    }

    public SymmetricEncryption(PrivateKey priv, PublicKey pub) throws GeneralSecurityException {
        
        this(EllipticCryptography.secp256k1.generateSharedKey(priv, pub, keySize));
    }

    public byte[] encrypt(byte[] plaintext) throws GeneralSecurityException {
        
        byte[] emptyArray = new byte[0];
        try{
            random.nextBytes(iv);
            AlgorithmParameterSpec paramSpec = getAlgorithmParameterSpec();

            cipher.init(Cipher.ENCRYPT_MODE, keySpec , paramSpec, random);
            
            return Arrays.concatenate(cipher.doFinal(plaintext), iv);
        }
        catch(GeneralSecurityException e){
            return emptyArray;
        }  

    }

    //Decrypt returns an empty byte array if the ciphertext is invalid. Invalid ciphertext would 
    //otherwise cause an exception as the algorithm would be unable to authenticate the ciphertext.
    public byte[] decrypt(byte[] encrypted) {    
        
        byte[] emptyArray = new byte[0];
        try{
            int messageLength = encrypted.length - ivSize;
            if(messageLength<0){
                //System.out.println("The ciphertext does not have sufficient length to contain the iv (initialisation vector) which should have been appended.");
                return emptyArray;
            }
            iv = Arrays.copyOfRange(encrypted, messageLength , encrypted.length);
            AlgorithmParameterSpec paramSpec = getAlgorithmParameterSpec();
            
            cipher.init(Cipher.DECRYPT_MODE, keySpec, paramSpec , random);
            
            return cipher.doFinal(Arrays.copyOfRange(encrypted, 0, messageLength));
        }
        catch(GeneralSecurityException e){
            return emptyArray;
        }
    }

    public int getKeySize(){
        return keySize;
    }

    private AlgorithmParameterSpec getAlgorithmParameterSpec(){
        
        return new GCMParameterSpec(tagSize * 8, iv);
    }

    

}
