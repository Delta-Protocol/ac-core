package us.wallet;

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

public class EllipticCryptography {
    private SecureRandom secureRandom;
    private KeyFactory factory;
    private ECParameterSpec ecSpec;
    private ECDomainParameters curve;
    private KeyPairGenerator generator;


    public static EllipticCryptography secp256k1;
    static {
        try {
        secp256k1=new EllipticCryptography("secp256k1", "ECDSA", "SHA256withECDSA");
        }
        catch (GeneralSecurityException e) {
        }
    }

    String signatureAlgorithm;

    public EllipticCryptography(String curveName, String algorithmName, String signatureAlgo) throws GeneralSecurityException {
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

    public byte[] generateSharedKey(PrivateKey priv, PublicKey pub, int length) throws NoSuchProviderException, InvalidKeyException, InvalidKeySpecException, NoSuchAlgorithmException {
        ECPrivateKey ecPrivateKey = (ECPrivateKey) priv;
        ECPublicKey ecPublicKey = (ECPublicKey) pub;

        ECPoint newPoint = ecMultiply(ecPublicKey.getQ(), ecPrivateKey.getD());
        byte[] encodedPoint = newPoint.getEncoded(true);
        MessageDigest messageDigest = MessageDigest.getInstance("SHA-256");
        byte[] hashed = messageDigest.digest(encodedPoint);
        return Arrays.copyOf(hashed, 16);
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

    public boolean verify(PublicKey pub, byte[] message, byte[] hash ) throws GeneralSecurityException {
        Signature dsa = Signature.getInstance(signatureAlgorithm);
        dsa.initVerify(pub);
        dsa.update(message);
        return dsa.verify(hash);
    }

    public byte[] sign(PrivateKey priv, byte[] message) throws GeneralSecurityException {
        Signature dsa = Signature.getInstance(signatureAlgorithm);
        dsa.initSign(priv);
        dsa.update(message);
        return dsa.sign();
    }

}
