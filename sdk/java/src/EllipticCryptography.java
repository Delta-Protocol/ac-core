package us.wallet;

import javax.crypto.KeyAgreement;
import org.spongycastle.jce.spec.ECParameterSpec;
import java.security.spec.ECGenParameterSpec;
import org.spongycastle.jce.ECNamedCurveTable;
import org.spongycastle.math.ec.ECPoint;
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

public class EllipticCryptography {

    private static EllipticCryptography instance;
    private SecureRandom secureRandom;
    private static KeyFactory factory;
    private static ECParameterSpec curve_params=null;
    private ECDomainParameters curve=null;
   


    private EllipticCryptography() throws NoSuchProviderException, InvalidKeyException, NoSuchAlgorithmException,InvalidAlgorithmParameterException, InvalidKeySpecException{
        
        Security.addProvider(new BouncyCastleProvider());
        curve_params=ECNamedCurveTable.getParameterSpec("secp256k1");
        factory = KeyFactory.getInstance("ECDSA");
        curve = new ECDomainParameters(curve_params.getCurve(), curve_params.getG(), curve_params.getN(), curve_params.getH());
        secureRandom = new SecureRandom();
        
    }

    public static EllipticCryptography getInstance() throws NoSuchProviderException, InvalidKeyException, NoSuchAlgorithmException,InvalidAlgorithmParameterException, InvalidKeySpecException {
        if(instance == null) {
            instance = new EllipticCryptography();
        }
        return instance;
    }

    public BigInteger generatePrivateKey() throws NoSuchProviderException, NoSuchAlgorithmException,InvalidAlgorithmParameterException, InvalidKeySpecException{
        KeyPair keyPair = generateKeyPair();
        return getBigIntegerFromKeyPair(keyPair);
        
    }

    private BigInteger getBigIntegerFromKeyPair(KeyPair keypair){
        return new BigInteger(keypair.getPrivate().getEncoded());
    }

    public PrivateKey privateKeyFromKeyPair(KeyPair keypair) throws InvalidKeySpecException{
        
        return factory.generatePrivate(new PKCS8EncodedKeySpec(keypair.getPrivate().getEncoded()));
    }

    private BigInteger getBigIntegerFromPrivateKey(PrivateKey privateKey){
        return new BigInteger(privateKey.getEncoded());
    }

    public PublicKey publicKeyFromKeyPair(KeyPair keypair) throws InvalidKeySpecException {
        return factory.generatePublic(new X509EncodedKeySpec(keypair.getPublic().getEncoded()));
    }

    public static KeyPair generateKeyPair() throws NoSuchAlgorithmException,InvalidAlgorithmParameterException, NoSuchProviderException {

        KeyPairGenerator generator = KeyPairGenerator.getInstance("ECDSA");
        generator.initialize(curve_params);
        return generator.generateKeyPair();
    }

    public static byte[] generateSharedKey(PrivateKey privKeyA, PublicKey pubKeyB) throws NoSuchProviderException, InvalidKeyException, InvalidKeySpecException, NoSuchAlgorithmException {
        KeyAgreement aKA = KeyAgreement.getInstance("ECDH");
        aKA.init(privKeyA);
        aKA.doPhase(pubKeyB, true);
        return aKA.generateSecret();
    }
    /*private PublicKey publicKeyFromBytes(Byte[] publicKey){
        return factory.generatePublic(new X509EncodedKeySpec(
    }*/
    public ECPoint publicPointFromPrivate(BigInteger privKey) {
        /*
         * TODO: FixedPointCombMultiplier currently doesn't support scalars longer than the group order,
         * but that could change in future versions.
         */
        if (privKey.bitLength() > curve_params.getN().bitLength()) {
            privKey = privKey.mod(curve_params.getN());
        }
        return new FixedPointCombMultiplier().multiply(curve_params.getG(), privKey);
    }
}