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


import javax.crypto.KeyAgreement;
import java.security.spec.ECGenParameterSpec;
import org.spongycastle.jce.ECNamedCurveTable;
import org.spongycastle.math.ec.FixedPointCombMultiplier;
import java.math.BigInteger;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.SecureRandom;
import java.security.KeyFactory;
import java.security.KeyPair;
import java.security.KeyPairGenerator;
import java.security.spec.X509EncodedKeySpec;
import java.security.spec.PKCS8EncodedKeySpec;
import org.spongycastle.jce.provider.BouncyCastleProvider;
import org.spongycastle.crypto.params.ECKeyGenerationParameters;
import org.spongycastle.crypto.params.ECDomainParameters;
import org.spongycastle.util.test.FixedSecureRandom;
import org.spongycastle.asn1.x9.X9ECParameters;
import java.security.Security;
import java.security.InvalidKeyException;
import java.security.spec.InvalidKeySpecException;
import java.security.NoSuchAlgorithmException;
import java.security.InvalidAlgorithmParameterException;
import java.security.NoSuchProviderException;
import java.util.Base64;
import javax.xml.bind.DatatypeConverter;
import org.spongycastle.util.BigIntegers;

public class EllipticCryptography {

    private static EllipticCryptography instance;
    private SecureRandom secureRandom;
    private static KeyFactory factory;
    private static ECParameterSpec ecSpec;
    private ECDomainParameters curve;
    private static KeyPairGenerator generator;

    private EllipticCryptography() throws NoSuchProviderException, InvalidKeyException, NoSuchAlgorithmException,InvalidAlgorithmParameterException, InvalidKeySpecException{
        
        Security.addProvider(new BouncyCastleProvider());

        String curveName = "secp256k1";
        String algorithmName = "ECDSA";
        ecSpec = ECNamedCurveTable.getParameterSpec(curveName);
        factory = KeyFactory.getInstance(algorithmName);
        curve = new ECDomainParameters(ecSpec.getCurve(), ecSpec.getG(), ecSpec.getN(), ecSpec.getH());
        secureRandom = new SecureRandom();
        generator = KeyPairGenerator.getInstance(algorithmName);
        generator.initialize(ecSpec);  

    }

    public static EllipticCryptography getInstance() throws NoSuchProviderException, InvalidKeyException, NoSuchAlgorithmException,InvalidAlgorithmParameterException, InvalidKeySpecException {
        
        if(instance == null) {
            instance = new EllipticCryptography();
        }
        return instance;
    }

    public static BigInteger generatePrivateKey() throws NoSuchProviderException, NoSuchAlgorithmException,InvalidAlgorithmParameterException, InvalidKeySpecException{
        KeyPair keyPair = generateKeyPair();
        return getBigIntegerPrivateKey(keyPair);  
    }

    private static BigInteger getBigIntegerPrivateKey(KeyPair keypair){
        return getBigIntegerPrivateKey(keypair.getPrivate().getEncoded());
    }

    private static BigInteger getBigIntegerPrivateKey(PrivateKey privateKey){
        return getBigIntegerPrivateKey(privateKey.getEncoded());
    }

    private static BigInteger getBigIntegerPrivateKey(byte[] privateKey){
        return new BigInteger(privateKey);
    }
 
    public static PrivateKey getPrivateKey(BigInteger privateKey) throws InvalidKeySpecException{
        ECPrivateKeySpec priKeySpec = new ECPrivateKeySpec(privateKey, ecSpec);
        return factory.generatePrivate(priKeySpec);
    }

    public static PrivateKey privateKeyFromKeyPair(KeyPair keypair) throws InvalidKeySpecException {
        return factory.generatePrivate(new PKCS8EncodedKeySpec(keypair.getPrivate().getEncoded()));
    }

    public static PublicKey publicKeyFromKeyPair(KeyPair keypair) throws InvalidKeySpecException {
        return factory.generatePublic(new X509EncodedKeySpec(keypair.getPublic().getEncoded()));
    }

    public static KeyPair generateKeyPair() throws NoSuchAlgorithmException,InvalidAlgorithmParameterException, NoSuchProviderException {
        return generator.generateKeyPair();
    }

    public static byte[] generateSharedKey(PrivateKey priv, PublicKey pub, int length) throws NoSuchProviderException, InvalidKeyException, InvalidKeySpecException, NoSuchAlgorithmException {
        ECPrivateKey ecPrivateKey = (ECPrivateKey) priv;
        ECPublicKey ecPublicKey = (ECPublicKey) pub;
        
        ECPoint newPoint = ecMultiply(ecPublicKey.getQ(), ecPrivateKey.getD());
        byte[] encodedPoint = newPoint.getEncoded(true);
        MessageDigest messageDigest = MessageDigest.getInstance("SHA-256");
        byte[] hashed = messageDigest.digest(encodedPoint);
        return Arrays.copyOf(hashed, 16);
    }
    
    public static PrivateKey getPrivateKey(byte[] privateKey) throws InvalidKeySpecException{
    
        ECPrivateKeySpec privKeySpec = new ECPrivateKeySpec(BigIntegers.fromUnsignedByteArray(privateKey),ecSpec);
        return factory.generatePrivate(privKeySpec);
    }

    private static ECPoint getECPoint(byte[] publicKey){
        ECCurve curve = ecSpec.getCurve();
        return curve.decodePoint(publicKey);
    }

    public static PublicKey getPublicKey(byte[] publicKey) throws InvalidKeySpecException{
        ECPoint ecPoint = getECPoint(publicKey);
        return getPublicKey(ecPoint);
        
    }

    public static PublicKey getPublicKey(ECPoint ecPoint)  throws InvalidKeySpecException{
        ECPublicKeySpec pubKeySpec = new ECPublicKeySpec(ecPoint, ecSpec);
        return factory.generatePublic(pubKeySpec);
    }

    public static ECPoint ecMultiply(ECPoint ecPoint, BigInteger d ){
        if (d.bitLength() > ecSpec.getN().bitLength()) {
            d = d.mod(ecSpec.getN());
        }
        return ecPoint.multiply(d);
    }
 
    public static BigInteger getPrivateInt(PrivateKey privateKey){
        ECPrivateKey ecPrivateKey = (ECPrivateKey) privateKey;
        return ecPrivateKey.getD();
    }
    public static PublicKey getPublicKeyFromPrivate(PrivateKey privateKey) throws InvalidKeySpecException{
        
        BigInteger privateInt = getPrivateInt(privateKey);
        return getPublicKeyFromPrivate(privateInt);

    }

    public static PublicKey getPublicKeyFromPrivate(BigInteger privateKey) throws InvalidKeySpecException{
        
        ECPoint ecPoint = ecMultiply(ecSpec.getG(), privateKey);
        return getPublicKey(ecPoint);

    }

}