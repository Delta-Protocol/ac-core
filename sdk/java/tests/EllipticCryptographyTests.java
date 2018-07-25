package us.test;

import us.wallet.*;
import java.security.PrivateKey;
import org.spongycastle.jce.spec.ECPrivateKeySpec;
import org.spongycastle.jce.spec.ECPublicKeySpec;
import org.spongycastle.jce.spec.ECParameterSpec;
import org.spongycastle.crypto.params.ECPrivateKeyParameters;
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
            testKeyConversion();
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

    private void testKeyConversion() throws GeneralSecurityException{
        KeyPair a = ec.generateKeyPair();

        PrivateKey priv_a = a.getPrivate();
        PublicKey pub_a = a.getPublic();
        PrivateKey priv_b = b.getPrivate();
        PublicKey pub_b = b.getPublic();


        byte[] key1 = ec.generateSharedKey(priv_a, pub_b,16);

        //PublicKey pub_b2 = ec.publicKeyAndBackAgain(pub_a);
        
        //ECPrivateKeyParameters privParams_b = (ECPrivateKeyParameters) priv_b.getPrivate();
        
       // throw new GeneralSecurityException("privs encoded: " + priv_a.getEncoded() + " " + priv_b.getEncoded() + "\n" + privParams_a.getD() + " " + privParams_a.getD());
       
    }

    private boolean testSharedSecret() throws GeneralSecurityException {
        KeyPair a = ec.generateKeyPair();
        KeyPair b = ec.generateKeyPair();

        PrivateKey priv_a = a.getPrivate();
        PublicKey pub_a = a.getPublic();
        PrivateKey priv_b = b.getPrivate();
        PublicKey pub_b = b.getPublic();

        byte[] key1 = ec.generateSharedKey(priv_a, pub_b,16);
        byte[] key2 = ec.generateSharedKey(priv_b, pub_a,16);
        
        return Arrays.equals(key1,key2);

    }
}