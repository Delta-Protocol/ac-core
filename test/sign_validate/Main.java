import us.gov.crypto.base58;
import us.gov.crypto.ec;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.KeyPair;
import java.security.GeneralSecurityException;
import java.util.Arrays;
import java.math.BigInteger;
import org.spongycastle.math.ec.ECPoint;
//import javax.xml.bind.DatatypeConverter;


public class Main{
    public static void main(String [ ] args) throws GeneralSecurityException {
//	System.out.println(args.length);
       if (args.length > 2){
            try{
                byte[] key=base58.decode(args[0]);
                String command = args[1];
                String message = args[2];

                if(command.equals("sign")){

                    PrivateKey privateKey = ec.instance.getPrivateKey(key);

                    String sig = ec.instance.sign_encode(privateKey,message);
                    System.out.println(sig);
                }
//		System.out.println("0:" + args[0]);
//		System.out.println("1:" + args[1]);
//		System.out.println("2:" + args[2]);
//		System.out.println("3:" + args[3]);

                if(command.equals("verify")){
//			System.out.println("verify");
                    PublicKey publicKey = ec.instance.getPublicKey(key);
                    String der_b58=args[3];
                    boolean validated = ec.instance.verify(publicKey, message, der_b58);

                    System.out.println(validated);
                }
            }
            catch(GeneralSecurityException e){
                System.out.println("error in java code: " + e);
            }
        }



    }



}
