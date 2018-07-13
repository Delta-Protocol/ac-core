package us.test;

import us.wallet.*;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.KeyPair;
import java.security.GeneralSecurityException;
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

    private boolean testSharedSecret() throws GeneralSecurityException {
        KeyPair a = ec.generateKeyPair();
        KeyPair b = ec.generateKeyPair();

        PrivateKey priv_a = ec.privateKeyFromKeyPair(a);
        PublicKey pub_a = ec.publicKeyFromKeyPair(a);
        PrivateKey priv_b = ec.privateKeyFromKeyPair(b);
        PublicKey pub_b = ec.publicKeyFromKeyPair(b);

        byte[] key1 = ec.generateSharedKey(priv_a, pub_b);
        byte[] key2 = ec.generateSharedKey(priv_b, pub_a);
        
        return Arrays.equals(key1,key2);

    }
}