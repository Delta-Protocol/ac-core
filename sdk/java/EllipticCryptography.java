import org.spongycastle.asn1.x9.X9ECParameters;
import org.spongycastle.crypto.AsymmetricCipherKeyPair;
import org.spongycastle.crypto.ec.CustomNamedCurves;
import org.spongycastle.crypto.generators.ECKeyPairGenerator;
import org.spongycastle.crypto.params.ECDomainParameters;
import org.spongycastle.crypto.params.ECKeyGenerationParameters;
import org.spongycastle.crypto.params.ECPrivateKeyParameters;
import org.spongycastle.crypto.params.ECPublicKeyParameters;
import org.spongycastle.math.ec.ECPoint;
import org.spongycastle.math.ec.FixedPointCombMultiplier;
import org.spongycastle.math.ec.FixedPointUtil;

public class EllipticCryptography{

    private static EllipticCryptography instance = null;
    private EllipticCryptography(){}
    public static EllipticCryptography getInstance() {
        if(instance == null) {
           instance = new EllipticCryptography();
        }
        return instance;
    }
    private static final SecureRandom secureRandom;
    private static final X9ECParameters curve_params = CustomNamedCurves.getByName("secp256k1");
    private static final ECDomainParameters curve;
    static {
        FixedPointUtil.precompute(curve_params.getG(), 12);
        curve = new ECDomainParameters(curve_params.getCurve(), curve_params.getG(), curve_params.getN(), curve_params.getH());
        secureRandom = new SecureRandom();
    }

    public SecretKey generatePrivateKey(){
            AsymmetricCipherKeyPair = generateKeyPair();
            ECPrivateKeyParameters privParams = (ECPrivateKeyParameters) keypair.getPrivate();
            return privParams.getD();
    }

    public AsymmetricCipherKeyPair generateKeyPair(){
        ECKeyPairGenerator generator = new ECKeyPairGenerator();
        ECKeyGenerationParameters keygenParams = new ECKeyGenerationParameters(curve, secureRandom);
        generator.init(keygenParams);
        return generator.generateKeyPair();
    }

    public static SecretKey generateSharedKey(SecretKey privKeyA, ECPoint pubKeyB)
            throws Exception {
                KeyAgreement aKA = KeyAgreement.getInstance("ECDH", "SC");
                aKA.init(privKeyA);
                aKA.doPhase(pubKeyB, true);

            return aKA.generateSecret();
    }

    public static ECPoint publicPointFromPrivate(BigInteger privKey) {
        /*
         * TODO: FixedPointCombMultiplier currently doesn't support scalars longer than the group order,
         * but that could change in future versions.
         */
        if (privKey.bitLength() > curve.getN().bitLength()) {
            privKey = privKey.mod(curve.getN());
        }
        return new FixedPointCombMultiplier().multiply(curve.getG(), privKey);
    }
}