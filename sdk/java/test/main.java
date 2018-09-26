import us.gov.crypto.test.symmetric_encryption;
import us.gov.crypto.test.ec;
import us.gov.crypto.base58;
import java.security.KeyPair;
import java.security.*;
import us.gov.cash.*;
import us.wallet.*;

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
        PrivateKey privateKey = us.gov.crypto.ec.instance.getPrivateKey(base58.decode("Gxp2aWgFB6SoyU79LhBfe71EE4yGVRe84tabJsSa7zRF"));
        KeyPair k = us.gov.crypto.ec.instance.generateKeyPair(privateKey);

        System.out.println("Private key: " +us.gov.crypto.ec.instance.to_b58(k.getPrivate()));
        System.out.println("Public key: " +us.gov.crypto.ec.instance.to_b58(k.getPublic()));

        short port=16673;
        us.wallet.daemon_rpc_api api=new us.wallet.daemon_rpc_api(k,"127.0.0.1",port);
        //api.balance(false,System.out);
        //api.list(false,System.out);
        //api.new_address(System.out);
        //api.add_address("5vvscYyiYzSMxEdPVqsPstB9wugHuanp6ts1FSWAsvFk", System.out);
        //api.transfer("gU5xx5Cvbm7NpPKz7TvdvBQmzXa",1,System.out);
        
        //tx_make_p2pkh_input i = new tx_make_p2pkh_input("gU5xx5Cvbm7NpPKz7TvdvBQmzXa",10,1,tx.sigcode_t.sigcode_all,tx.sigcode_t.sigcode_all,false);
        //api.tx_make_p2pkh(i, System.out);
         
        //api.tx_sign("gU5xx5Cvbm7NpPKz7TvdvBQmzXa", tx.sigcode_t.sigcode_all, tx.sigcode_t.sigcode_all, System.out);
        //api.tx_send("gU5xx5Cvbm7NpPKz7TvdvBQmzXa", System.out);
        //api.tx_decode("gU5xx5Cvbm7NpPKz7TvdvBQmzXa", System.out);
        //api.tx_check("gU5xx5Cvbm7NpPKz7TvdvBQmzXa", System.out);
        
        //api.ping(System.out);
        
        //api.list_devices(System.out);
        //api.pair("26smcjHgGsGgvUEUpoCKpUnwHhL5W9y48Wtz7tryn5Yab", "test", System.out);
        api.unpair("26smcjHgGsGgvUEUpoCKpUnwHhL5W9y48Wtz7tryn5Yab", System.out);
        System.out.println();

        }
        catch(Exception e) {
            System.out.println("FAILED:" + e);
            Thread.dumpStack();
        }
    }
}
