package com.example.usgov;

import java.io.FileOutputStream;

import android.content.Context;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;

import org.spongycastle.asn1.*;
import org.spongycastle.asn1.x9.X9ECParameters;
import org.spongycastle.asn1.x9.X9IntegerConverter;
import org.spongycastle.crypto.AsymmetricCipherKeyPair;
import org.spongycastle.crypto.digests.SHA256Digest;
import org.spongycastle.crypto.ec.CustomNamedCurves;
import org.spongycastle.crypto.generators.ECKeyPairGenerator;
import org.spongycastle.crypto.params.*;
import org.spongycastle.crypto.signers.ECDSASigner;
import org.spongycastle.crypto.signers.HMacDSAKCalculator;
import org.spongycastle.math.ec.ECAlgorithms;
import org.spongycastle.math.ec.ECPoint;
import org.spongycastle.math.ec.FixedPointCombMultiplier;
import org.spongycastle.math.ec.FixedPointUtil;
import org.spongycastle.math.ec.custom.sec.SecP256K1Curve;
import org.spongycastle.util.Properties;
import org.spongycastle.util.encoders.Base64;
import java.security.SecureRandom;
import java.math.BigInteger;

public class MainActivity extends AppCompatActivity {
    /*
    static {
        Security.insertProviderAt(new org.spongycastle.jce.provider.BouncyCastleProvider(), 1);
    }
*/
    public static final X9ECParameters curve_params = CustomNamedCurves.getByName("secp256k1");
    public static final ECDomainParameters curve; // The parameters of the secp256k1 curve that Bitcoin uses.


    public BigInteger priv;
    public LazyECPoint pub;

    void on_refresh() {
        String msg="balance";
        String signature="";
        //create signature
        String query=msg+" "+pub+" "+signature; //query to our node (answering is walletd@node, our own node which recognises our identity, we must ha$
        //send to node. IP address in pairing data;
    }

    void on_balance() {
        // update balance in textbox
    }

    void pay(String address,  int amount, int fees) {
        String msg="pay "+address+" "+amount+" "+fees;
        String signature="";
        //create signature
        String query=msg+" "+pub+" "+signature; //query to our node (answering is walletd@node, our own node which recognises our identity, we must ha$
        //send to node. IP address in pairing data;
    }
    void on_tx(String signed_tx) {

    }

    void pay(int amount) {
        String msg="pay ";
        String address; // coming from nfc_data.

    }


    void on_pay() {
        int amount=0; //inputbox amount
        pay(amount);
    }


    private static final SecureRandom secureRandom;


    static {
        //if (Utils.isAndroidRuntime()) new LinuxSecureRandom();
        FixedPointUtil.precompute(curve_params.getG(), 12);
        curve= new ECDomainParameters(curve_params.getCurve(), curve_params.getG(), curve_params.getN(), curve_params.getH());
        secureRandom = new SecureRandom();
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, "Replace with your own action", Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });


        ECKeyPairGenerator generator = new ECKeyPairGenerator();
        ECKeyGenerationParameters keygenParams = new ECKeyGenerationParameters(curve, secureRandom);
        generator.init(keygenParams);
        AsymmetricCipherKeyPair keypair = generator.generateKeyPair();
        ECPrivateKeyParameters privParams = (ECPrivateKeyParameters) keypair.getPrivate();
        ECPublicKeyParameters pubParams = (ECPublicKeyParameters) keypair.getPublic();
        priv = privParams.getD();
        pub = new LazyECPoint(curve.getCurve(), pubParams.getQ().getEncoded(true));


	String filename = "k";
	String fileContents = priv.toString();

	FileOutputStream outputStream;

	try {
	    outputStream = openFileOutput(filename, Context.MODE_PRIVATE);
	    outputStream.write(fileContents.getBytes());
	    outputStream.close();
	} catch (Exception e) {
	    e.printStackTrace();
	}



    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
