package us.gov.crypto;

import org.spongycastle.jce.spec.ECParameterSpec;
import org.spongycastle.jce.spec.ECPrivateKeySpec;
import org.spongycastle.jce.spec.ECPublicKeySpec;
import org.spongycastle.math.ec.ECPoint;
import org.spongycastle.jce.spec.ECNamedCurveParameterSpec;
import org.spongycastle.jce.spec.ECNamedCurveSpec;
import org.spongycastle.jce.ECPointUtil;
import java.security.MessageDigest;
import org.spongycastle.math.ec.ECCurve;
import java.util.Arrays;
import org.spongycastle.jce.interfaces.ECPublicKey;
import org.spongycastle.jce.interfaces.ECPrivateKey;
import org.spongycastle.crypto.params.ECDomainParameters;
import org.spongycastle.jce.ECNamedCurveTable;
import java.security.Signature;
import java.math.BigInteger;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.SecureRandom;
import java.security.KeyFactory;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import org.spongycastle.jce.provider.BouncyCastleProvider;
import java.security.Security;
import java.security.InvalidKeyException;
import java.security.spec.InvalidKeySpecException;
import java.security.NoSuchAlgorithmException;
import java.security.GeneralSecurityException;
import java.security.InvalidAlgorithmParameterException;
import java.security.NoSuchProviderException;
import org.spongycastle.util.BigIntegers;

public class ec {
    private SecureRandom secureRandom;
    private KeyFactory factory;
    private ECParameterSpec ecSpec;
    private ECDomainParameters curve;
    private KeyPairGenerator generator;


//    public static ec secp256k1;
    public static ec instance;
    static {
        try {
        instance=new ec("secp256k1", "ECDSA", "SHA256withECDSA");
        //secp256k1=new ec("secp256k1", "ECDSA", "SHA256withECDSA");
        }
        catch (GeneralSecurityException e) {
        }
    }

    String signatureAlgorithm;

    public ec(String curveName, String algorithmName, String signatureAlgo) throws GeneralSecurityException {
            signatureAlgorithm=signatureAlgo;
            Security.addProvider(new BouncyCastleProvider());
            ecSpec = ECNamedCurveTable.getParameterSpec(curveName);
            factory = KeyFactory.getInstance(algorithmName);
            curve = new ECDomainParameters(ecSpec.getCurve(), ecSpec.getG(), ecSpec.getN(), ecSpec.getH());
            secureRandom = new SecureRandom();
            generator = KeyPairGenerator.getInstance(algorithmName);
            generator.initialize(ecSpec);
    }

    public BigInteger generatePrivateInt() throws NoSuchProviderException, NoSuchAlgorithmException,InvalidAlgorithmParameterException, InvalidKeySpecException {
        KeyPair keyPair = generateKeyPair();
        return getPrivateInt(keyPair);
    }

    private BigInteger getPrivateInt(KeyPair keypair) {
        return getPrivateInt(keypair.getPrivate());
    }

    public BigInteger getPrivateInt(PrivateKey privateKey) {
        ECPrivateKey ecPrivateKey = (ECPrivateKey) privateKey;
        return ecPrivateKey.getD();
    }

    public PrivateKey getPrivateKey(BigInteger privateKey) throws InvalidKeySpecException {
        ECPrivateKeySpec priKeySpec = new ECPrivateKeySpec(privateKey, ecSpec);
        return factory.generatePrivate(priKeySpec);
    }

    public KeyPair generateKeyPair() throws NoSuchAlgorithmException,InvalidAlgorithmParameterException, NoSuchProviderException {
        return generator.generateKeyPair();
    }

    public KeyPair generateKeyPair(PrivateKey k) {
        try {
            return new KeyPair(getPublicKeyFromPrivate(k),k);
        }
        catch(Exception e) {
            return null;
        }
    }

    public byte[] generateSharedKey(PrivateKey priv, PublicKey pub, int length) throws NoSuchProviderException, InvalidKeyException, InvalidKeySpecException, NoSuchAlgorithmException {
        ECPrivateKey ecPrivateKey = (ECPrivateKey) priv;
        ECPublicKey ecPublicKey = (ECPublicKey) pub;

        ECPoint newPoint = ecMultiply(ecPublicKey.getQ(), ecPrivateKey.getD());
        byte[] encodedPoint = newPoint.getEncoded(true);
        MessageDigest messageDigest = MessageDigest.getInstance("SHA-256");
        byte[] hashed = messageDigest.digest(encodedPoint);
        return Arrays.copyOf(hashed, 16);
    }

    public byte[] ripemd160(String text) {
        try {
            MessageDigest messageDigest = MessageDigest.getInstance("RIPEMD160");
            return messageDigest.digest(text.getBytes());
        } catch(Exception e) {
            return null;
        }
    }

    public byte[] sha256(String text) {
        try {
            MessageDigest messageDigest = MessageDigest.getInstance("SHA-256");
            return messageDigest.digest(text.getBytes());
        } catch(Exception e) {
            return null;
        }
    }

    public byte[] getPublicKeyArray(PublicKey pub) throws NoSuchProviderException, InvalidKeyException, InvalidKeySpecException, NoSuchAlgorithmException {
        ECPublicKey ecPublicKey = (ECPublicKey) pub;

        ECPoint newPoint = ecPublicKey.getQ();
        byte[] encodedPoint = newPoint.getEncoded(true);
        return encodedPoint;
    }

    public String to_b58(PublicKey k) {
        try {
            return base58.encode(getPublicKeyArray(k));
        }
        catch(Exception e) {
            return null;
        }
    }
    public String to_b58(PrivateKey k) {
        try {
//System.out.println(k.getEncoded().length);
            return base58.encode(k.getEncoded());
        }
        catch(Exception e) {
            return null;
        }
    }

    public PrivateKey getPrivateKey(byte[] privateKey) throws InvalidKeySpecException {
        return getPrivateKey(BigIntegers.fromUnsignedByteArray(privateKey));
    }

    private ECPoint getECPoint(byte[] publicKey) {
        ECCurve curve = ecSpec.getCurve();
        return curve.decodePoint(publicKey);
    }

    public PublicKey getPublicKey(byte[] publicKey) throws InvalidKeySpecException {
        ECPoint ecPoint = getECPoint(publicKey);
        return getPublicKey(ecPoint);
    }

    public PublicKey getPublicKey(ECPoint ecPoint)  throws InvalidKeySpecException {
        ECPublicKeySpec pubKeySpec = new ECPublicKeySpec(ecPoint, ecSpec);
        return factory.generatePublic(pubKeySpec);
    }

    public ECPoint ecMultiply(ECPoint ecPoint, BigInteger d ) {
        if (d.bitLength() > ecSpec.getN().bitLength()) {
            d = d.mod(ecSpec.getN());
        }
        return ecPoint.multiply(d);
    }

    public PublicKey getPublicKeyFromPrivate(PrivateKey priv) throws InvalidKeySpecException {
        BigInteger privateInt = getPrivateInt(priv);
        return getPublicKeyFromPrivate(privateInt);
    }

    public PublicKey getPublicKeyFromPrivate(BigInteger priv) throws InvalidKeySpecException {
        ECPoint ecPoint = ecMultiply(ecSpec.getG(), priv);
        return getPublicKey(ecPoint);
    }

    public boolean verify(PublicKey pub, String text, String signature_der_b58 ) {
//System.out.println("java verify: pub" +to_b58(pub) + " text: " +text+ " sig: " + signature_der_b58);
        try {
            Signature dsa = Signature.getInstance(signatureAlgorithm);
            dsa.initVerify(pub);
            dsa.update(text.getBytes());
//System.out.println("java verify sha256(text): " +sha256(text));
            return dsa.verify(base58.decode(signature_der_b58));
        } catch(Exception e) {
            return false;
        }
    }

/*
    public boolean verify(PublicKey pub, byte[] message, String signature_der_b58 ) throws GeneralSecurityException {
        Signature dsa = Signature.getInstance(signatureAlgorithm);
        dsa.initVerify(pub);
        dsa.update(message);
        return dsa.verify(base58.decode(signature_der_b58));
    }
*/
    public byte[] sign(PrivateKey priv, String message) throws GeneralSecurityException {
        Signature dsa = Signature.getInstance(signatureAlgorithm);
        dsa.initSign(priv);
        dsa.update(message.getBytes());
        return dsa.sign();
    }

    public String sign_encode(PrivateKey priv, String message) throws GeneralSecurityException {
        return base58.encode(sign(priv,message));
    }

}
