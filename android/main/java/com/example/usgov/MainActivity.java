package com.example.usgov;

import android.os.Handler;
import android.app.AlertDialog;
import android.content.Context;
import android.os.Bundle;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.view.View;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Button;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;

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

import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.security.SecureRandom;
import java.math.BigInteger;
import java.nio.file.Files;
import java.nio.file.Paths;

import android.view.inputmethod.InputMethodManager;
import android.app.Activity;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    private static final SecureRandom secureRandom;

    public static final X9ECParameters curve_params = CustomNamedCurves.getByName("secp256k1");
    public static final ECDomainParameters curve; // The parameters of the secp256k1 curve that Bitcoin uses.
    static {
        //if (Utils.isAndroidRuntime()) new LinuxSecureRandom();
        FixedPointUtil.precompute(curve_params.getG(), 12);
        curve= new ECDomainParameters(curve_params.getCurve(), curve_params.getG(), curve_params.getN(), curve_params.getH());
        secureRandom = new SecureRandom();
    }

    public BigInteger priv;
    public ECPoint pub;
    String log="";

    public String convertStreamToString(InputStream is) throws Exception { //1 line
        log+=";A";
        BufferedReader reader = new BufferedReader(new InputStreamReader(is));
        log+=";B";

        StringBuilder sb = new StringBuilder();
        log+=";C";
        String line = null;
        while ((line = reader.readLine()) != null) {
            log+=";D";
            sb.append(line);
            log+=";E";
            break;
        }
        log+=";F";
        reader.close();
        log+=";G";
        return sb.toString();
    }

    public String getStringFromFile (File fl) {
        log+=";1";
        try {
            FileInputStream fin = new FileInputStream(fl);
            log+=";2";
            String ret = convertStreamToString(fin);
            log+=";3";
            //Make sure you close all streams.
            fin.close();
            log+=";4";
            return ret;
        }
        catch(Exception e) {
            log+=";5 "+e.getMessage();
            return "";//e.getStackTrace().toString();
        }
    }

    public static boolean isAndroidRuntime() {
            final String runtime = System.getProperty("java.runtime.name");
            return runtime != null && runtime.equals("Android Runtime");
    }

    public static ECPoint publicPointFromPrivate(BigInteger privKey) {
        /*
         * TODO: FixedPointCombMultiplier currently doesn't support scalars longer than the group order,
         * but that could change in future versions.
         */
        if (privKey.bitLength() > curve.getN().bitLength()) {
            privKey = privKey.mod(curve.getN());
        }
        return new FixedPointCombMultiplier().multiply(curve.getG(), privKey);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (isAndroidRuntime()) new LinuxSecureRandom(); //Asserts /dev/urandom is ok

        setContentView(R.layout.activity_main);
        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
        setSupportActionBar(toolbar);

        FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);

        //pub = new LazyECPoint(curve.getCurve(), pubParams.getQ().getEncoded(true));



        String filename = "k";


   //     log="files dir:"+getFilesDir();

        File file = new File(getFilesDir(),filename);
        if(!file.exists()) {
            file.getParentFile().mkdirs();
            try {
                file.createNewFile();
            }
            catch(Exception e) {
                log+=";EXCEPRT";
            }
 //           log+=";new file";
//            if (!file.mkdirs()) {
  //              //Log.e(LOG_TAG, "Directory not created");
    //            log+=";Directory not created";
      //      }
            ECKeyPairGenerator generator = new ECKeyPairGenerator();
            ECKeyGenerationParameters keygenParams = new ECKeyGenerationParameters(curve, secureRandom);
            generator.init(keygenParams);
            AsymmetricCipherKeyPair keypair = generator.generateKeyPair();
            ECPrivateKeyParameters privParams = (ECPrivateKeyParameters) keypair.getPrivate();
            ECPublicKeyParameters pubParams = (ECPublicKeyParameters) keypair.getPublic();
            priv = privParams.getD();
            String fileContents = priv.toString();
    //log+=";"+fileContents.length();
            FileOutputStream outputStream;

            try {
                outputStream = openFileOutput(filename, Context.MODE_PRIVATE);
                outputStream.write(fileContents.getBytes());
                outputStream.write('\n');
                outputStream.close();
                log+=";wrote "+fileContents.getBytes().length;
                        /*
                        File file2 = new File(filename);
                        if(!file2.exists()) {
                            log += ";file does not exist2";
                        }
                        */
            } catch (Exception e) {
                log+=e.getMessage();
            }

        }
        else {
            log+=";file exists+";

             String content = getStringFromFile(file);
            log+="+;content "+content;
            priv=new BigInteger(content);
            //log+="read: " + priv.toString();

        }
        //pub = new LazyECPoint(curve.getCurve(), pubParams.getQ().getEncoded(true));
        pub = publicPointFromPrivate(priv);
    //log=pub.getEncoded(true).toString();
        //pub=new LazyECPoint(priv);


        Button balance = (Button) findViewById(R.id.balance);
        balance.setText("balance");
        balance.setOnClickListener(this);

        final String flog=log;

        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, flog, Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });

        Button pay = (Button) findViewById(R.id.pay);

        pay.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                toggleContactless("Pay");
            }
        });

        Button get_paid = (Button) findViewById(R.id.get_paid);

        get_paid.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                toggleContactless("Charge ");

            }
        });



        findViewById(R.id.amount).setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (!hasFocus) {
                    hideKeyboard(v);
                }
            }
        });


    }

    public void toggleContactless(String verb) {
        EditText a = (EditText) findViewById(R.id.amount);
        TextView action = (TextView) findViewById(R.id.action);
        Button p = (Button) findViewById(R.id.pay);
        action.setText(verb+" " +a.getText());
        findViewById(R.id.amount).setVisibility(findViewById(R.id.amount).getVisibility() == View.VISIBLE ? View.INVISIBLE : View.VISIBLE);
        findViewById(R.id.action).setVisibility(findViewById(R.id.action).getVisibility() == View.VISIBLE ? View.INVISIBLE : View.VISIBLE);
        findViewById(R.id.contactless_logo).setVisibility(findViewById(R.id.contactless_logo).getVisibility() == View.VISIBLE ? View.INVISIBLE : View.VISIBLE);

        findViewById(R.id.get_paid).setVisibility(findViewById(R.id.get_paid).getVisibility() == View.VISIBLE ? View.INVISIBLE : View.VISIBLE);
        if (findViewById(R.id.get_paid).getVisibility() == View.VISIBLE)
            p.setText("PAY");
        else
            p.setText("CANCEL");


    }

    public void hideKeyboard(View view) {
        InputMethodManager inputMethodManager =(InputMethodManager)getSystemService(Activity.INPUT_METHOD_SERVICE);
        inputMethodManager.hideSoftInputFromWindow(view.getWindowToken(), 0);
    }

    @Override
    public void onClick(View v) {

        switch (v.getId()) {

            case R.id.balance:
                Button balance = (Button) findViewById(R.id.balance);
                if (balance.getText()=="balance") {
                    balance.setText("...");
                    final Handler handler = new Handler();
                    Thread thread = new Thread(new Runnable() {
                        @Override
                        public void run() {
                            String signature = "";
                            try {
                                //Replace below IP with the IP of that device in which server socket open.
                                //If you change port then change the port number in the server side code also.
                                Socket s = new Socket("92.51.240.61", 16673);
                                boolean b11 = s.isClosed();
                                boolean b12 = s.isConnected();
                                String msg = "BALANCE " + pub.getEncoded(true).toString() + " " + signature;
                                Datagram d = new Datagram(0, msg);
                                d.send(s);

                                boolean b11_2 = s.isClosed();
                                boolean b12_2 = s.isConnected();

                                //Datagram r=Datagram.recv(s);

                                String st;
                                Datagram r = new Datagram();
                                if (r.recv(s)) {
                                    st = r.parse_string();
                                } else {
                                    st = "?";
                                }
                                boolean b11_3 = s.isClosed();
                                boolean b12_3 = s.isConnected();
                                final String stf = st;

                                handler.post(new Runnable() {
                                    @Override
                                    public void run() {
                                        Button balance = (Button) findViewById(R.id.balance);
                                        balance.setText(cash_human.show(stf));
//                                    String s = mTextViewReplyFromServer.getText().toString();
//                                    if (st.trim().length() != 0)
                                        //                                      mTextViewReplyFromServer.setText(s + "\nFrom Server : " + st);
                                    }
                                });

                                //output.close();
                                //out.close();
                                s.close();
                            } catch (IOException e) {
                                //e.printStackTrace();
                            }
                        }
                    });

                    thread.start();
                }
                else {
                    balance.setText("balance");
                }


                break;
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
