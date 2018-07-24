import us.wallet.*;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.KeyPair;
import java.security.GeneralSecurityException;
import java.util.Arrays;
import java.math.BigInteger;
import org.spongycastle.math.ec.ECPoint;
import javax.xml.bind.DatatypeConverter;
import org.spongycastle.util.BigIntegers;


public class Main{
    public static void main(String [ ] args) throws GeneralSecurityException{
        if (args.length > 3){
            try{
            //PrivateKey priv_a = args[0];
            //PublicKey pub_b = args[1];
            //string command = args[2];
            //string ciphertext = args[3];
            System.out.println("java recieved public key " + args[1]);
            System.out.println("java recieved private key " + args[0]);

            byte[] pub=Base58.decode(args[1]);
            byte[] priv=Base58.decode(args[0]);
            
            byte[] privfix = EllipticCryptography.getInstance().toByteArray("5f706787ac72c1080275c1f398640fb07e9da0b124ae9734b28b8d0f01eda586");
            String priv_base58 = Base58.encode(privfix);
            System.out.println("base58_priv:" + priv_base58);
/*
            byte[] pubfix = toByteArray("04b80cdf1422644ccfb0a2c73103bdfa3cc96786c3e63d8df70267fc7fffe711a1000d37a20cefd1fdcceec0b0b3f25a46c8a430800ba0c19f4ae0cfc582de8fb8");
            String pub_base58 = Base58.encode(pubfix);
            System.out.println("base58_pub:" + pub_base58);
            */
            System.out.println("priv key bytes to string " + Arrays.toString(priv));
            
            BigInteger privInt2 = BigIntegers.fromUnsignedByteArray(priv);
            System.out.println("private bigint should be : 59246973681978278211771777659815725362199838110377467354329990080200723911781");
            
            
            System.out.println("priv ints = " + privInt2);

            PrivateKey privateKey = EllipticCryptography.getInstance().newGetPrivateKey(priv);
            PublicKey publicKey = EllipticCryptography.getInstance().newGetPublicKey(pub);
           
            System.out.println("check pub hex: " + EllipticCryptography.getInstance().toHexString(pub));
            byte[] sharedKey = EllipticCryptography.getInstance().generateSharedKey(privateKey,publicKey,16);
            //byte[] sharedKeysame = EllipticCryptography.getInstance().generateSharedKey(privateKey,publicKey, 16);
          
            ECPoint ecPoint2 = EllipticCryptography.getInstance().publicPointFromPrivate(privInt2);
            PublicKey public2 = EllipticCryptography.getInstance().newGetPublicKey(ecPoint2);
            byte[] sharedKey2 = EllipticCryptography.getInstance().generateSharedKey(privateKey,public2, 16);
        
            //System.out.println("java shared key: " + Base58.encode(sharedKey));
            //System.out.println("java shared key bytes: " + Arrays.toString(sharedKey));
            //System.out.println("java shared key same: " + Base58.encode(sharedKeysame));
            //System.out.println("java shared key: " + Base58.encode(sharedKey2));

            
            }
            catch(GeneralSecurityException e){
                System.out.println("error in java code: " + e);
            }
        }

    

    }

}
