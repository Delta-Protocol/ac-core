package us.wallet;

import org.spongycastle.asn1.x9.X9ECParameters;
import org.spongycastle.crypto.AsymmetricCipherKeyPair;
import org.spongycastle.crypto.ec.CustomNamedCurves;
import org.spongycastle.crypto.generators.ECKeyPairGenerator;
import javax.crypto.KeyAgreement;
import org.spongycastle.crypto.params.ECKeyGenerationParameters;
import org.spongycastle.crypto.agreement.ECDHBasicAgreement;
import org.spongycastle.crypto.params.ECPrivateKeyParameters;
import org.spongycastle.math.ec.ECPoint;
import org.spongycastle.math.ec.FixedPointCombMultiplier;
import org.spongycastle.math.ec.FixedPointUtil;
import javax.crypto.SecretKey;
import java.math.BigInteger;
import java.security.PublicKey;
import java.security.SecureRandom;
import org.spongycastle.crypto.params.ECDomainParameters;
import org.spongycastle.util.test.FixedSecureRandom;

public class EllipticCryptography {

    private static EllipticCryptography instance = null;
    private SecureRandom secureRandom=null;
    private X9ECParameters curve_params=null;
    private ECDomainParameters curve=null;


    private EllipticCryptography() {
        curve_params=CustomNamedCurves.getByName("secp256k1");
        FixedPointUtil.precompute(curve_params.getG(), 12);
        curve = new ECDomainParameters(curve_params.getCurve(), curve_params.getG(), curve_params.getN(), curve_params.getH());
        secureRandom = new SecureRandom();
    }

    public static EllipticCryptography getInstance() {
        if(instance == null) {
           instance = new EllipticCryptography();
        }
        return instance;
    }

    public BigInteger generatePrivateKey() {
            AsymmetricCipherKeyPair keypair = generateKeyPair();
            ECPrivateKeyParameters privParams = (ECPrivateKeyParameters) keypair.getPrivate();
            return privParams.getD();
    }

    public AsymmetricCipherKeyPair generateKeyPair(){
        ECKeyPairGenerator generator = new ECKeyPairGenerator();
        ECKeyGenerationParameters keygenParams = new ECKeyGenerationParameters(curve, secureRandom);
        generator.init(keygenParams);
        return generator.generateKeyPair();
    }

    public static byte[] generateSharedKey(SecretKey privKeyA, ECPoint pubKeyB) throws Exception {
                KeyAgreement aKA = KeyAgreement.getInstance("ECDH", "SC");
                aKA.init(privKeyA);
                aKA.doPhase((PublicKey)pubKeyB, true);
                return aKA.generateSecret();
    }

    public ECPoint publicPointFromPrivate(BigInteger privKey) {
        /*
         * TODO: FixedPointCombMultiplier currently doesn't support scalars longer than the group order,
         * but that could change in future versions.
         */
        if (privKey.bitLength() > curve.getN().bitLength()) {
            privKey = privKey.mod(curve.getN());
        }
        return new FixedPointCombMultiplier().multiply(curve_params.getG(), privKey);
    }
}
