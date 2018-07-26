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
            
       
                String command = args[2];
                byte[] message = Base58.decode(args[3]);

                byte[] pub=Base58.decode(args[1]);
                byte[] priv=Base58.decode(args[0]);

                
                //System.out.println("message " + Arrays.toString(message));
               

                PrivateKey privateKey = EllipticCryptography.getInstance().newGetPrivateKey(priv);
                PublicKey publicKey = EllipticCryptography.getInstance().newGetPublicKey(pub);
   
                byte[] sharedKey = EllipticCryptography.getInstance().newGenerateSharedKey(privateKey,publicKey, 16);
                
                SymmetricEncryption se = new SymmetricEncryption(privateKey,publicKey);
        
                if(command.equals("decrypt")){
                    
                    
                    byte[] decrypted = se.decrypt(message);
                    String decrypted_string = Arrays.toString(decrypted);
                    System.out.println("decrypted as: " + decrypted_string);
                }
                if(command.equals("encrypt")){
                    
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

    private static void printHex(byte[] byteArray){
        System.out.println(EllipticCryptography.toHexString(byteArray));
    }

}
