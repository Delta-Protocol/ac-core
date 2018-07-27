package us.test;

import us.wallet.*;
import java.security.PrivateKey;
import org.spongycastle.jce.spec.ECPrivateKeySpec;
import org.spongycastle.jce.spec.ECPublicKeySpec;
import org.spongycastle.jce.spec.ECParameterSpec;
import org.spongycastle.crypto.params.ECPrivateKeyParameters;
import java.security.PublicKey;
import java.security.KeyPair;
import java.security.NoSuchProviderException;
import java.security.NoSuchAlgorithmException;
import java.security.InvalidAlgorithmParameterException;
import java.security.spec.InvalidKeySpecException;
import java.security.GeneralSecurityException;
import java.math.BigInteger;

import java.util.Arrays;

public class EllipticCryptographyTests{

    private static EllipticCryptography ec;
        
        KeyPair a;
        KeyPair b;
        KeyPair c;

    public boolean testEllipticCryptography() throws GeneralSecurityException {
        
        try{
            ec = EllipticCryptography.getInstance();
            assert(testKeyPairGeneration());
            assert(testSharedSecret());
            //assert(testKeyConversions());
        }
        catch(GeneralSecurityException e){
            throw new GeneralSecurityException("elliptic cryptography test failed on: " + e);
        }
        return true;
    }


    private boolean testKeyPairGeneration() throws GeneralSecurityException{
        
        KeyPair a = ec.generateKeyPair();
        KeyPair b = ec.generateKeyPair();

        // add code to check keypairs aren't equal
        return true;
    }
/*
    private boolean testKeyConversion1(){
        BigInteger privInts = ec.generatePrivateKey();
        PrivateKey priv = ec.getPrivateKey(privInts);
        
        return true;
    }


    private boolean testKeyConversion2() throws NoSuchAlgorithmException, NoSuchProviderException, InvalidAlgorithmParameterException, InvalidKeySpecException{
        
        byte[] privateKeyBytes = Base58.decode("4ACKBcXXhtGb3NtZzSfwgSs3GqCgCBY65juF2UVSJQR2");
        byte[] publicKeyBytes = Base58.decode("dPtUg631Hh7tL8t3wK6KHDwFfQzLmcHtkEAH5mSsANX3");

        PrivateKey privateKey = ec.getPrivateKey(privateKeyBytes);
        PublicKey publicKey = ec.getPublicKey(publicKeyBytes);

        PublicKey publicKey2 = ec.getPublicKeyFromPrivate(privateKey);
       
        //byte[] publicKeyBytes2 = 
        //System.out.println("pub2 bytes" + publicKeyBytes2);
        System.out.println("pub bytes" + publicKeyBytes);
        //return Arrays.equals(publicKeyBytes,publicKeyBytes2);
        return true;

    }
*/
    private boolean testSharedSecret() throws GeneralSecurityException {
        KeyPair a = ec.generateKeyPair();
        KeyPair b = ec.generateKeyPair();

        PrivateKey priv_a = a.getPrivate();
        PublicKey pub_a = a.getPublic();
        PrivateKey priv_b = b.getPrivate();
        PublicKey pub_b = b.getPublic();
        
        return testSharedSecret(priv_a, pub_a, priv_b, pub_b);
    }

    private boolean testSharedSecret(PrivateKey priv_a, PublicKey pub_a, PrivateKey priv_b, PublicKey pub_b) throws GeneralSecurityException {

        byte[] key1 = ec.generateSharedKey(priv_a, pub_b, 16);
        byte[] key2 = ec.generateSharedKey(priv_b, pub_a, 16);
        
        return Arrays.equals(key1,key2);
    }
}