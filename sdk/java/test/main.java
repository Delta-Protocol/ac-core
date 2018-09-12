import us.gov.crypto.test.symmetric_encryption;
import us.gov.crypto.test.ec;
import us.gov.crypto.base58;
import java.security.KeyPair;
import java.security.*;

public class main {

    public static void main(String[] args)  {
/*
        try{
            symmetric_encryption se = new symmetric_encryption();
            se.testSymmetricEncryption();

            ec ec = new ec();
            ec.testEllipticCryptography();

            System.out.println("Java tests PASSED");
        }
        catch(Exception e){
            System.out.println("Java tests FAILED:" + e);
        }
*/
        try {

        //KeyPair k=us.gov.crypto.ec.instance.generateKeyPair();
        PrivateKey privateKey = us.gov.crypto.ec.instance.getPrivateKey(base58.decode("5vvscYyiYzSMxEdPVqsPstB9wugHuanp6ts1FSWAsvFk"));
        KeyPair k = us.gov.crypto.ec.instance.generateKeyPair(privateKey);

        System.out.println("Private key: " +us.gov.crypto.ec.instance.to_b58(k.getPrivate()));
        System.out.println("Public key: " +us.gov.crypto.ec.instance.to_b58(k.getPublic()));

        short port=16673;
        us.wallet.daemon_rpc_api api=new us.wallet.daemon_rpc_api(k,"127.0.0.1",port);
        api.balance(false,System.out);

        }
        catch(Exception e) {
            System.out.println("FAILED:" + e);
            Thread.dumpStack();
        }
    }
}
