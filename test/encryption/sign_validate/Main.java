import us.wallet.*;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.GeneralSecurityException;


public class Main{
    public static void main(String [ ] args) throws GeneralSecurityException {
       if (args.length > 2){
            try{
                
                byte[] key=Base58.decode(args[0]);
                String command = args[1];
                byte[] message = args[2].getBytes();
                if(command.equals("sign")){
                    
                    PrivateKey privateKey = EllipticCryptography.secp256k1.getPrivateKey(key);
                    byte[] signed = EllipticCryptography.secp256k1.sign(privateKey,message);
                    
                    System.out.println(Base58.encode(signed));
                }
                if(command.equals("verify")){
               
                    PublicKey publicKey = EllipticCryptography.secp256k1.getPublicKey(key);
                    byte[] hash =Base58.decode(args[3]);
                    boolean validated = EllipticCryptography.secp256k1.verify(publicKey, message, hash);
                    
                    System.out.println(validated);
                }
            }
            catch(GeneralSecurityException e){
                System.out.println("error in java code: " + e);
            }
        }

    

    }

   

}
