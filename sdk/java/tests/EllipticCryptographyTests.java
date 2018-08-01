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

         KeyPair a;
        KeyPair b;
        KeyPair c;

    public boolean testEllipticCryptography() throws GeneralSecurityException {
        
        try{
           
            assert(testSharedSecret());
            assert(testSharedSecretRawKeys());
            assert(testKeyConversion());
            assert(testSignAndVerify());
        }
        catch(GeneralSecurityException e){
            throw new GeneralSecurityException("elliptic cryptography test failed on: " + e);
        }
        return true;
    }

    //tests that conversion between raw private key and java PrivateKey type is successful.
    private boolean testKeyConversion() throws InvalidKeySpecException, NoSuchProviderException, NoSuchAlgorithmException, InvalidAlgorithmParameterException {
        
        BigInteger privInts = EllipticCryptography.generatePrivateInt();
        PrivateKey priv = EllipticCryptography.getPrivateKey(privInts);
        BigInteger privInts2 = EllipticCryptography.getPrivateInt(priv);
        
        return privInts==privInts2;
    }

    //tests shared secret generation with real raw key values.
    private boolean testSharedSecretRawKeys() throws GeneralSecurityException{
        PrivateKey privateKey = EllipticCryptography.getPrivateKey(Base58.decode("9pKQDhcZsi9V1qVhaDDnqV7HyiatxUEwTiLjqtqD7ZR6"));
        PublicKey publicKey = EllipticCryptography.getPublicKey(Base58.decode("or69BumA7ZALzHNKjuxDLtHithXo3BfzJ2VYg73uNizk"));
 
        byte[] key = EllipticCryptography.generateSharedKey(privateKey, publicKey, 16);
        ;
        return Base58.encode(key).equals("C1w1ufPafGrkgPCZRFRT1x");
    }

    //tests that two keypairs will generate the same shared secret using their own private key and the other's public key.
    private boolean testSharedSecret() throws GeneralSecurityException {
        
        KeyPair a = EllipticCryptography.generateKeyPair();
        KeyPair b = EllipticCryptography.generateKeyPair();

        PrivateKey priv_a = a.getPrivate();
        PublicKey pub_a = a.getPublic();
        PrivateKey priv_b = b.getPrivate();
        PublicKey pub_b = b.getPublic();

        byte[] key1 = EllipticCryptography.generateSharedKey(priv_a, pub_b, 16);
        byte[] key2 = EllipticCryptography.generateSharedKey(priv_b, pub_a, 16);
        
        return Arrays.equals(key1,key2);
    }

    private boolean testSignAndVerify() throws GeneralSecurityException {
        
        KeyPair a = EllipticCryptography.generateKeyPair();
        String message_string = "How do you know that this message is from who you think it is?";
        byte[] message = message_string.getBytes();
        byte[] signature = EllipticCryptography.sign(a.getPrivate(), message);

        return EllipticCryptography.verify(a.getPublic(), message, signature);
    }

}