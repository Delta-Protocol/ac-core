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
            
                //System.out.println("java recieved public key " + args[1]);
                //System.out.println("java recieved private key " + args[0]);
                String command = args[2];
                byte[] ciphertext = Base58.decode(args[3]);

                byte[] pub=Base58.decode(args[1]);
                byte[] priv=Base58.decode(args[0]);

                System.out.println("command " + command);
                
                System.out.println("message " + ciphertext);
                
                //byte[] privfix = EllipticCryptography.getInstance().toByteArray("5f706787ac72c1080275c1f398640fb07e9da0b124ae9734b28b8d0f01eda586");
                /*byte[] privfix = EllipticCryptography.getInstance().toByteArray("82fc9947e878fc7ed01c6c310688603f0a41c8e8704e5b990e8388343b0fd465");
                String priv_base58 = Base58.encode(privfix);
                System.out.println("base58_priv:" + priv_base58);
                    
                byte[] pubfix = EllipticCryptography.getInstance().toByteArray("02b80cdf1422644ccfb0a2c73103bdfa3cc96786c3e63d8df70267fc7fffe711a1");
                String pub_base58 = Base58.encode(pubfix);
                System.out.println("base58_pub:" + pub_base58);*/
                
            // System.out.println("priv key bytes to string " + Arrays.toString(priv));
                
                //BigInteger privInt = BigIntegers.fromUnsignedByteArray(priv);
            
                
                //System.out.println("priv ints = " + privInt);

                PrivateKey privateKey = EllipticCryptography.getInstance().newGetPrivateKey(priv);
                PublicKey publicKey = EllipticCryptography.getInstance().newGetPublicKey(pub);
            
                //System.out.println("check pub hex: " + EllipticCryptography.getInstance().toHexString(pub));
            // byte[] sharedKey = EllipticCryptography.getInstance().generateSharedKey(privateKey,publicKey,16);
                //byte[] sharedKeysame = EllipticCryptography.getInstance().generateSharedKey(privateKey,publicKey, 16);
            
                //ECPoint ecPoint = EllipticCryptography.getInstance().publicPointFromPrivate(privInt);
                //PublicKey publicKey2 = EllipticCryptography.getInstance().newGetPublicKey(ecPoint);
                byte[] sharedKey = EllipticCryptography.getInstance().newGenerateSharedKey(privateKey,publicKey, 16);
                //byte[] sharedKey2 = EllipticCryptography.getInstance().newGenerateSharedKey(privateKey,publicKey2, 16);
            
                //System.out.println("java shared key: " + Arrays.toString(sharedKey));
                //System.out.println("java shared key2: " + Arrays.toString(sharedKey2));
                //System.out.println("java shared key bytes: " + Arrays.toString(sharedKey));
                //System.out.println("java shared key: " + Base58.encode(sharedKey));

                //System.out.println("java shared key2: " + Base58.encode(sharedKey2));
                //SymmetricEncryption se = new SymmetricEncryption(sharedKey);
                SymmetricEncryption se = new SymmetricEncryption("fakekey123456789".getBytes());
                
                byte[] decrypted = se.decrypt(ciphertext);
                String decrypted_string = Arrays.toString(decrypted);
                System.out.println("decrypted as: " + decrypted_string);
            }
            catch(GeneralSecurityException e){
                System.out.println("error in java code: " + e);
            }
        }

    

    }

}
