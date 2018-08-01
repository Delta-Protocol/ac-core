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
        if (args.length > 3){
            try{
       
                byte[] priv=Base58.decode(args[0]);
                byte[] pub=Base58.decode(args[1]);
                String command = args[2];
               
                PrivateKey privateKey = EllipticCryptography.getPrivateKey(priv);
                PublicKey publicKey = EllipticCryptography.getPublicKey(pub);
   
                SymmetricEncryption se = new SymmetricEncryption(privateKey, publicKey);
        
                if(command.equals("decrypt")){
                    
                    byte[] message = Base58.decode(args[3]);
                    byte[] decrypted = se.decrypt(message);
                    String decrypted_string = new String(decrypted);
                    System.out.println(decrypted_string);
                }
                if(command.equals("encrypt")){
                    
                    byte[] message = args[3].getBytes();
                    byte[] encrypted = se.encrypt(message);
                    String encrypted_string = Arrays.toString(encrypted);
                   
                    System.out.println(Base58.encode(encrypted));
                }
            }
            catch(GeneralSecurityException e){
                System.out.println("error in java code: " + e);
            }
        }

    

    }

   

}
