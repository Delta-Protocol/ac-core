import us.wallet.*;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.KeyPair;
import java.security.GeneralSecurityException;
import java.util.Arrays;
import java.math.BigInteger;
import org.spongycastle.math.ec.ECPoint;
import javax.xml.bind.DatatypeConverter;


public class Main{
    public static void main(String [ ] args) throws GeneralSecurityException{
        if (args.length > 1){
            try{
                byte[] key=Base58.decode(args[0]);
                String command = args[1];
                byte[] message = args[2].getBytes();

                if(command.equals("sign")){
                    
                    PrivateKey privateKey = EllipticCryptography.getPrivateKey(key);
                    
                    byte[] signed = EllipticCryptography.sign(privateKey,message);
                    String encoded = new String(Base58.encode(signed));
                    System.out.println(encoded);
                }
                if(command.equals("verify")){
               
                    PublicKey publicKey = EllipticCryptography.getPublicKey(key);
                    byte[] hash =Base58.decode(args[3]);
                    boolean validated = EllipticCryptography.verify(publicKey, message, hash);
                    
                    System.out.println(validated);
                }
            }
            catch(GeneralSecurityException e){
                System.out.println("error in java code: " + e);
            }
        }

    

    }

   

}
