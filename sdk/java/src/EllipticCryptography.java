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
    //private static ECNamedCurveParameterSpec spec;
    private static ECParameterSpec ecSpec;
    //private static ECNamedCurveSpec params;
    private ECDomainParameters curve;
    private static KeyPairGenerator generator;

   


    private EllipticCryptography() throws NoSuchProviderException, InvalidKeyException, NoSuchAlgorithmException,InvalidAlgorithmParameterException, InvalidKeySpecException{
        
        Security.addProvider(new BouncyCastleProvider());

        //ECParameterSpec params = new ECNamedCurveSpec(ECNamedCurveTable.getParameterSpec("prime239v1"));
        String curveName = "secp256k1";
        ecSpec = ECNamedCurveTable.getParameterSpec(curveName);
        //spec = ECNamedCurveTable.getParameterSpec(curveName);
        //params = new ECNamedCurveSpec(curveName,spec.getCurve(), spec.getG(), spec.getN());
        factory = KeyFactory.getInstance("ECDSA");
        curve = new ECDomainParameters(ecSpec.getCurve(), ecSpec.getG(), ecSpec.getN(), ecSpec.getH());
        secureRandom = new SecureRandom();
        generator = KeyPairGenerator.getInstance("ECDSA");
        generator.initialize(ecSpec);  
        

    }

    public static EllipticCryptography getInstance() throws NoSuchProviderException, InvalidKeyException, NoSuchAlgorithmException,InvalidAlgorithmParameterException, InvalidKeySpecException {
        
        if(instance == null) {
            instance = new EllipticCryptography();
        }
        return instance;
    }
/*
    public static byte[] hexStringToByteArray(String hexString) {
        byte[] bytes = new byte[hexString.length() / 2];
    
        for(int i = 0; i < hexString.length(); i += 2){
            String sub = hexString.substring(i, i + 2);
            Integer intVal = Integer.parseInt(sub, 16);
            bytes[i / 2] = intVal.byteValue();
            String hex = "".format("0x%x", bytes[i / 2]);
        }
        return bytes;
    }
    
    private static byte[] P256_HEAD = Base64.getDecoder().decode("MFYwEAYHKoZIzj0CAQYFK4EEAAoDQgAE");

    public static PublicKey convertPublicKey(byte[] w) throws InvalidKeySpecException, NoSuchAlgorithmException {
        byte[] encodedKey = new byte[P256_HEAD.length + w.length];
        System.arraycopy(P256_HEAD, 0, encodedKey, 0, P256_HEAD.length);
        System.arraycopy(w, 0, encodedKey, P256_HEAD.length, w.length);
        KeyFactory eckf = KeyFactory.getInstance("EC");
        
        X509EncodedKeySpec ecpks = new X509EncodedKeySpec(encodedKey);
        return eckf.generatePublic(ecpks);
    }
    */
    /*
    public PublicKey newGetPublicKeyFromEncoded(byte[] encoded){
        KeyFactory eckf = KeyFactory.getInstance("EC");
        X509EncodedKeySpec ecpks = new X509EncodedKeySpec(encodedKey);
        return eckf.generatePublic(ecpks);
    }
    */

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

    public static PrivateKey getPrivateKey(KeyPair keypair) throws InvalidKeySpecException{
        return getPrivateKey(keypair.getPrivate().getEncoded());
    }
    public static PrivateKey getPrivateKey(BigInteger privateKey) throws InvalidKeySpecException{
        ECPrivateKeySpec priKeySpec = new ECPrivateKeySpec(privateKey, ecSpec);
        return factory.generatePrivate(priKeySpec);
    }
    public static PrivateKey getPrivateKey(byte[] privateKey) throws InvalidKeySpecException{
        return getPrivateKey(new BigInteger(privateKey));
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

    public static byte[] generateSharedKey(PrivateKey privKeyA, PublicKey pubKeyB, int length) throws NoSuchProviderException, InvalidKeyException, InvalidKeySpecException, NoSuchAlgorithmException {
        KeyAgreement aKA = KeyAgreement.getInstance("ECDH");
        aKA.init(privKeyA);
        aKA.doPhase(pubKeyB, true);
        byte[] secret = aKA.generateSecret();

        MessageDigest messageDigest = MessageDigest.getInstance("SHA-256");
        byte[] hashed = messageDigest.digest(secret);
        return Arrays.copyOf(hashed, 16);
        
    }

    public static byte[] newGenerateSharedKey(PrivateKey privKeyA, PublicKey pubKeyB, int length) throws NoSuchProviderException, InvalidKeyException, InvalidKeySpecException, NoSuchAlgorithmException {
        
        byte[] secret = aKA.generateSecret();

        MessageDigest messageDigest = MessageDigest.getInstance("SHA-256");
        byte[] hashed = messageDigest.digest(secret);
        return Arrays.copyOf(hashed, 16);
        
    }

    //new
    public PrivateKey newGetPrivateKey(byte[] privateKey) throws InvalidKeySpecException{
    
        ECPrivateKeySpec privKeySpec = new ECPrivateKeySpec(BigIntegers.fromUnsignedByteArray(privateKey),ecSpec);
        return factory.generatePrivate(privKeySpec);
    }
    private ECPoint newGetECPoint(byte[] publicKey){
        ECCurve curve = ecSpec.getCurve();
        return curve.decodePoint(publicKey);
    }

    public PublicKey newGetPublicKey(byte[] publicKey) throws InvalidKeySpecException{
        ECPoint ecPoint = newGetECPoint(publicKey);
        System.out.println("ecpoint x from pub key bytes: " + ecPoint.getX().toString());
        return newGetPublicKey(ecPoint);
        
    }

    public PublicKey newGetPublicKey(ECPoint ecPoint)  throws InvalidKeySpecException{
        ECPublicKeySpec pubKeySpec = new ECPublicKeySpec(ecPoint, ecSpec);
        return factory.generatePublic(pubKeySpec);
    }

    public ECPoint ecMultiply(ECPoint ecPoint, BigInteger d ){
        if (d.bitLength() > ecSpec.getN().bitLength()) {
            d = d.mod(ecSpec.getN());
        }
        
        return ecPoint.multiply(d);
    }
    /*private PublicKey publicKeyFromBytes(Byte[] publicKey){
        return factory.generatePublic(new X509EncodedKeySpec(
    }*/
    public ECPoint publicPointFromPrivate(BigInteger privKey) {
        
        return ecMultiply(ecSpec.getG(),privKey);
    }

    public static String toHexString(byte[] array) {
        return DatatypeConverter.printHexBinary(array);
    }
    
    public static byte[] toByteArray(String s) {
        return DatatypeConverter.parseHexBinary(s);
    }
}