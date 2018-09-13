import javax.crypto.KeyAgreement;
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
import java.security.spec.ECGenParameterSpec;
import java.security.Security;
import java.security.InvalidKeyException;
import java.security.spec.InvalidKeySpecException;
import java.security.NoSuchAlgorithmException;
import java.security.InvalidAlgorithmParameterException;
import java.security.NoSuchProviderException;
import java.io.IOException;
import java.util.Base64;
import java.util.Arrays;
import org.spongycastle.jce.provider.BouncyCastleProvider;

public class PrintHeadNamedCurve{

    //Print head

    public static void main(String [ ] args)  {
        Security.addProvider(new BouncyCastleProvider());
        try{
            printHeadForP256();
        }
        catch(Exception e){
            System.out.println("unable to print head. Error: " + e.toString());
        }
    }

    private static void printHeadForP256() throws NoSuchAlgorithmException, InvalidAlgorithmParameterException, IOException{
        String name = "secp256k1";
        int size = 256;
        byte[] head = createHeadForNamedCurve(name, size);
        System.out.println(Base64.getEncoder().encodeToString(head));
    }

    private static byte[] createHeadForNamedCurve(String name, int size) throws NoSuchAlgorithmException, InvalidAlgorithmParameterException, IOException {
        KeyPairGenerator kpg = KeyPairGenerator.getInstance("EC");
        ECGenParameterSpec m = new ECGenParameterSpec(name);
        kpg.initialize(m);
        KeyPair kp = kpg.generateKeyPair();
        byte[] encoded = kp.getPublic().getEncoded();
        return Arrays.copyOf(encoded, encoded.length - 2 * (size / Byte.SIZE));
    }
}