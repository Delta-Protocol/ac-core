package com.example.usgov;

import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.res.Configuration;
import android.nfc.NdefRecord;
import android.nfc.NdefMessage;
import android.nfc.NfcAdapter;
import android.nfc.NfcEvent;
import android.nfc.NfcAdapter.CreateNdefMessageCallback;
import android.nfc.NfcAdapter.OnNdefPushCompleteCallback;

import android.nfc.Tag;
import android.nfc.tech.IsoDep;
import android.os.Handler;
import android.app.AlertDialog;
import android.content.Context;
import android.os.Bundle;
import android.os.IBinder;
import android.os.Parcelable;
import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.support.v7.app.AppCompatActivity;
import android.support.v7.widget.Toolbar;
import android.util.Log;
import android.view.View;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.widget.Button;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import android.widget.Toast;

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
import java.nio.charset.Charset;
import java.security.SecureRandom;
import java.math.BigInteger;
import java.nio.file.Files;
import java.nio.file.Paths;

import java.util.Locale;
import java.util.Random;

import android.view.inputmethod.InputMethodManager;
import android.app.Activity;
import android.widget.EditText;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity implements View.OnClickListener {
    boolean bounded;
    HostCardEmulatorService svc;

    public class smartCardReader implements NfcAdapter.ReaderCallback { // CreateNdefMessageCallback, OnNdefPushCompleteCallback {
        Activity a;
        NfcAdapter mNfcAdapter;
        boolean reading=false;
        String verb;


        boolean is_reading() {
            return reading;
        }

        smartCardReader(Activity ac) {
            a=ac;
            mNfcAdapter = NfcAdapter.getDefaultAdapter(a);
            if (mNfcAdapter == null) {
                Toast.makeText(a, "NFC is not available", Toast.LENGTH_LONG).show();
//                finish();
                return;
            }
            // Register callback
            //mNfcAdapter.setNdefPushMessageCallback(this, a);
        }

        boolean nfc_available() {
            return mNfcAdapter != null;
        }

        /*
        void processIntent(Intent intent) {
            Parcelable[] rawMsgs = intent.getParcelableArrayExtra(NfcAdapter.EXTRA_NDEF_MESSAGES);
            // only one message sent during the beam
            NdefMessage msg = (NdefMessage) rawMsgs[0];
            // record 0 contains the MIME type, record 1 is the AAR, if present
            Toast.makeText(a, new String(msg.getRecords()[0].getPayload()), Toast.LENGTH_LONG).show();

        }
*/
/*
https://medium.com/the-almanac/how-to-build-a-simple-smart-card-emulator-reader-for-android-7975fae4040f

HEADER:00-A4-04-00
CLA: the Class byte is used to indicate to what extent the command complies with the ISO7816, and if so, what kind of “Secure Messaging” will be used. To keep things simple, we will not be using this byte in our example and we will be passing `00` all the time.
INS: the Instruction byte is used to indicate what method we want to execute, this can be a variety of methods like: `A4` to Select a File or Applet, `B0` to Read a Binary, `D0` to Write a Binary … (see full list of Instructions http://www.cardwerk.com/smartcards/smartcard_standard_ISO7816-4_5_basic_organizations.aspx#table11)
P1 & P2: these two Parameter bytes are used for further customization of the Instruction, they depend on what custom commands the card specifies. Click here for the list of the possible cases

 */

        byte[] APDUCommand(String payload) {
            int sz=4+1+HostCardEmulatorService.AID.length+payload.length()+1;
            byte[] r=new byte[sz];
            r[0]=HostCardEmulatorService.CLA;
            r[1]=HostCardEmulatorService.INS;
            r[2]=(byte)0x04;
            r[3]=(byte)0x00;

            r[4]=(byte)(HostCardEmulatorService.AID.length+payload.length());
            for (int i=0; i<HostCardEmulatorService.AID.length; ++i) {
                r[5+i]=HostCardEmulatorService.AID[i];
            }
            byte p[]=payload.getBytes();
            for (int i=0; i<p.length; ++i) {
                    r[5+HostCardEmulatorService.AID.length+i]=p[i];
            }
            r[sz-1]=HostCardEmulatorService.LE;
            return r;
        }




        @Override
        public void onTagDiscovered(Tag tag) {
            Log.d("usgov", "onTagDiscovered "+tag.toString());
            //1-2673/com.example.usgov D/usgov: onTagDiscovered TAG: Tech [android.nfc.tech.IsoDep, android.nfc.tech.NfcA]

            //String addr="23TeQWLmU5MUruhZc4SQuXhv6kHa";
            String addr="";
            Log.d("usgov", "APDUCommand "+HostCardEmulatorService.ByteArrayToHexString(APDUCommand(addr)));
            try {
                IsoDep isoDep = IsoDep.get(tag);
                isoDep.connect();



                //00A4040007A0000002471001
                final byte[] response = isoDep.transceive(APDUCommand(addr));
                runOnUiThread(new Thread(new Runnable() {
                    public void run() {
                        Log.d("usgov","CardResponse: " + Utils.Companion.toHex(response));
                        //05-17 08:22:26.946 2431-2431/com.example.usgov D/usgov: CardResponse: 6A82
                    }
                }));
                isoDep.close();
            }
            catch(IOException e) {
                Log.d("usgov", "onTagDiscovered "+tag.toString()+ "crashed "+e.getMessage());
            }
        }

/*
        public NdefRecord createMimeRecord(String payload) {
            NdefRecord mimeRecord = new NdefRecord(
                    NdefRecord.TNF_MIME_MEDIA ,
                    "application/com.example.usgov".getBytes(Charset.forName("US-ASCII")),
                    new byte[0], payload.getBytes(Charset.forName("US-ASCII")));
            return mimeRecord;
        }
        public NdefRecord createAR() {
            return NdefRecord.createApplicationRecord("com.example.usgov");
        }

        @Override
        public NdefMessage createNdefMessage(NfcEvent event) {
            Toast.makeText(a, "nfc_cb callback createNdefMessage", Toast.LENGTH_LONG).show();

            NdefMessage msg = new NdefMessage(
                    new NdefRecord[]{
                            createMimeRecord("A abcdefabcdef"),
                            createAR()
                    });
            return msg;
        }

*/
        public void no_intent() {
            if (!nfc_available()) return;
            if (reading) {
                Log.d("ZZZ","no_intent. stop reading");
                mNfcAdapter.disableReaderMode(a);
                reading = false;
                verb="";
            }
        }


        public void pay_intent(String v, String amount) {
            if (!nfc_available()) return;
            if (!reading) {
                Log.d("ZZZ","pay_intent. start reading");
                mNfcAdapter.enableReaderMode(a, this, NfcAdapter.FLAG_READER_NFC_A | NfcAdapter.FLAG_READER_SKIP_NDEF_CHECK, null);
                reading = true;
            }
            verb=v;
        }
/*
        @Override
        public void onNdefPushComplete(NfcEvent event) {
            Toast.makeText(a, "onNdefPushComplete", Toast.LENGTH_LONG).show();
        }
        */
    }

    private smartCardReader reader;

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

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);
/*
        // Checks the orientation of the screen
        if (newConfig.orientation == Configuration.ORIENTATION_LANDSCAPE) {
            Toast.makeText(this, "landscape", Toast.LENGTH_SHORT).show();
        } else if (newConfig.orientation == Configuration.ORIENTATION_PORTRAIT){
            Toast.makeText(this, "portrait", Toast.LENGTH_SHORT).show();
        }
        */
        //Log.d("XXX","calling UpdateControls from onConfigurationChanged");
        //updateControls();
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

    void constructor() {

    }

    int state=0;
    void transition_state(int st) {
        if (st==state) return;
        state=st;
        switch (state) {
            case 0:
                reader.no_intent();
                break;
            case 1: {
                EditText a = (EditText) findViewById(R.id.amount);
                reader.pay_intent("Charge", a.getText().toString());
            }
                break;
            case 2: {
                EditText a = (EditText) findViewById(R.id.amount);
                reader.pay_intent("Pay", a.getText().toString());
            }
                break;

        }
    }


    @Override
    protected void onSaveInstanceState(Bundle astate) {
        super.onSaveInstanceState(astate);
        Button balance = (Button) findViewById(R.id.balance);
        astate.putString("balancelbl", balance.getText().toString());
        astate.putInt("state", state);
        EditText a = (EditText) findViewById(R.id.amount);
        astate.putString("amount", a.getText().toString());

    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (isAndroidRuntime()) new LinuxSecureRandom(); //Asserts /dev/urandom is ok

        setContentView(R.layout.activity_main);

//        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
//        setSupportActionBar(toolbar);

  //      FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);

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
        //toggleContactless("AA");

        Button balance = (Button) findViewById(R.id.balance);
        if(savedInstanceState != null) {
            balance.setText(savedInstanceState.getString("balancelbl"));
        }
        else {
            balance.setText("balance");
        }
        balance.setOnClickListener(this);

        final String flog=log;
/*
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, flog, Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });
*/
        Button pay = (Button) findViewById(R.id.pay);
        pay.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (reader.is_reading()) {
                    transition_state(0);
                }
                else {
                    transition_state(2);
                }
                Log.d("XXX","calling UpdateControls from pay onclick");
                updateControls();
            }
        });

        Button get_paid = (Button) findViewById(R.id.get_paid);
        get_paid.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.d("XXX","calling UpdateControls from getpaid onclick");
                updateControls();
                transition_state(1);

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

        findViewById(R.id.get_paid).setEnabled(false);
        findViewById(R.id.pay).setEnabled(false);

        findViewById(R.id.amount).setOnKeyListener(new View.OnKeyListener() {
            @Override

            public boolean onKey(View v, int keyCode, KeyEvent event) {
                EditText a = (EditText) findViewById(R.id.amount);
                if (a.getText().length()==0) {
                    findViewById(R.id.get_paid).setEnabled(false);
                    findViewById(R.id.pay).setEnabled(false);

                } else {
                    findViewById(R.id.get_paid).setEnabled(true);
                    findViewById(R.id.pay).setEnabled(true);

                }
                return false;
            }
        });


        reader=new smartCardReader(this);
        if(savedInstanceState != null) {
            balance.setText(savedInstanceState.getString("balancelbl"));
            transition_state(savedInstanceState.getInt("state"));
        }
        else {
            balance.setText("balance");
            transition_state(0);
        }
        Log.d("XXX","calling UpdateControls from onCreate");

        updateControls();
    }

    @Override
    protected void onStart() {
        super.onStart();

        Intent mIntent = new Intent(this, HostCardEmulatorService.class);
        bindService(mIntent, mConnection, BIND_AUTO_CREATE);
    };
    @Override
    protected void onStop() {
        super.onStop();
        if(bounded) {
            unbindService(mConnection);
            bounded = false;
        }
    };

    ServiceConnection mConnection = new ServiceConnection() {
        @Override
        public void onServiceDisconnected(ComponentName name) {
            Toast.makeText(MainActivity.this, "Service is disconnected", 1000).show();
            bounded = false;
            svc = null;
        }

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            Toast.makeText(Client.this, "Service is connected", 1000).show();
            bounded = true;
            HostCardEmulatorService.LocalBinder mLocalBinder = (HostCardEmulatorService.LocalBinder)service;
            svc = mLocalBinder.getInstance();
        }
    };

    @Override
    public void onResume() {
        super.onResume();
        Log.d("XXX","calling UpdateControls from onResume");
        updateControls();
    }
    @Override
    public void onPause() {
        super.onPause();
        Log.d("XXX","calling UpdateControls from onPause");
        updateControls();
    }


    public String sign(String msg) {
        return "AN1rKvtbihLRkX6Utv2FjxLfehr43uqZr4eFUDvkZuNxmdjuecmSRmag3YFiXNu5HV8XiAkbERjmyCad1V556GqRLTmutYA5S";
    }

    public void on_tap() {
        boolean isPay=findViewById(R.id.get_paid).getVisibility() == View.VISIBLE;
        //pay protocol:
        //inet     node                         payer ===========NFC============ payee         node       inet
        // *        *<--------SEQ_REQ()----------- * -----PAY_INTENT(amount) ----> *             *          *
        // *        *---------random msg---------> *                               *             *          *
        // *        *                              *                               *             *          *
        // *        *                              * <----------address ---------- *             *          *
        // *        *<-TRANSFER_CASH(addr,amount)- *                               *             *          *
        // *        *---------signed tx ---------> *                               *             *          *
        // *<-relay-*                              * ----------signed tx --------> *             *          *
        // *        *                              *                               * ---tx------>*          *
        // *        *                              *                               *             * -relay-> *

        //String amount=

        //Send to payee a message PAY_INTENT with the amount
        EditText a = (EditText) findViewById(R.id.amount);
        String msg = "PAY_INTENT " + a.getText();
        //Send and receive NFC
        String rcptAddress="jGdHCR7xLb33dkAp7JcEu7bwu6g";
        //Send and receive to node

        String msg2 = "TRANSFER_CASH " + rcptAddress + " " + a.getText() + " " + pub.getEncoded(true).toString();
        String signature=sign(msg2);
        msg+=" "+signature;

        //received this transaction from the node
        String signedTx="2SEZqt5m9xFmSAa4cjKQPvRwCEd2YSVxifKAbQP3Ht1ubJH55zcHifq3JtQkP2nieHRVguJi7bE4sZCWpyrXcwfKaMrYK63Kr6mEU4aGpLg454sLdc4iXQ2aJH5CjN6v2LBPfGHUJBEQBQejH8GaJhz2sbeABjczsR2nKcrG61aKg9z5JPVRkLHtgoQatBhbGNqJ2c6GNi2kMPiDVgR1DorRTmjWsNmLH9zqzvfSEytkiN1KWdMVWN4n18aq9ZwL999WvoRucKkD2QT6shirc9zaLn5vNdEQ55m7uqDvtiVjgouL2dgE4HdJfDpsztQrsMqtQvWaxPLgDcK1HWXcbemsbDKT64CtnqdKyfrtkSDLwtps2FSedewbWotzEMybgcjZA88kYySmyno9mX6vGXzKVzWnZspFEG5h9KDHtGXisLQushWE7o1oD7X46q1qy6XvhcBQgF";
        //relay to the other mobile via NFC
    }

    //String verb=new String();

    public void updateControls() {
        boolean r=reader.is_reading();
Log.d("XXXXXXXX-UpdateControls",""+r);
        EditText a = (EditText) findViewById(R.id.amount);
        TextView action = (TextView) findViewById(R.id.action);
        action.setText(reader.verb+" " +cash_human.show(a.getText().toString()));
        findViewById(R.id.amount).setVisibility(r ? View.INVISIBLE : View.VISIBLE);
        findViewById(R.id.contactless_logo).setVisibility(r ? View.VISIBLE : View.INVISIBLE);
        action.setVisibility(r ? View.VISIBLE : View.INVISIBLE);
        findViewById(R.id.get_paid).setVisibility(r ? View.INVISIBLE : View.VISIBLE);
        Button p = (Button) findViewById(R.id.pay);
        if (reader.reading) {
            p.setText("CANCEL");
            findViewById(R.id.get_paid).setEnabled(true);
            findViewById(R.id.pay).setEnabled(true);
        }
        else {
            p.setText("PAY");
            if (a.getText().length()==0) {
                findViewById(R.id.get_paid).setEnabled(false);
                findViewById(R.id.pay).setEnabled(false);

            } else {
                findViewById(R.id.get_paid).setEnabled(true);
                findViewById(R.id.pay).setEnabled(true);

            }
        }
    }


    String nodeAddress() {
        return "11.11.11.11";
    }
    int nodePort() {
        return 16673;
    }


    public void hideKeyboard(View view) {
        InputMethodManager inputMethodManager =(InputMethodManager)getSystemService(Activity.INPUT_METHOD_SERVICE);
        inputMethodManager.hideSoftInputFromWindow(view.getWindowToken(), 0);
    }

    public static final int wallet_base = 0;

    public static final int protocol_balance_query = wallet_base+1;
    public static final int protocol_dump_query = wallet_base+2;
    public static final int protocol_new_address_query = wallet_base+3;
    public static final int protocol_add_address_query = wallet_base+4;
    public static final int tx_make_p2pkh_query = wallet_base+5;
    public static final int tx_sign_query = wallet_base+6;
    public static final int tx_send_query = wallet_base+7;
    public static final int tx_decode_query = wallet_base+8;
    public static final int tx_check_query = wallet_base+9;

    public static final int protocol_response = wallet_base+0;



/*
    String queryCashAddress() {
        final Handler handler = new Handler();
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                String signature = "";
                try {
                    Socket s = new Socket(nodeAddress(), nodePort());
                    boolean b11 = s.isClosed();
                    boolean b12 = s.isConnected();
                    String msg=pub.getEncoded(true).toString() + " " + sign(msg);
                    Datagram d = new Datagram(protocol_balance_query, msg);
                    d.send(s);

                    boolean b11_2 = s.isClosed();
                    boolean b12_2 = s.isConnected();

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
    String querySignedTx(String recipientAddress, int amount) {

    }

*/
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
                                Socket s = new Socket(nodeAddress(), nodePort());
                                boolean b11 = s.isClosed();
                                boolean b12 = s.isConnected();
                                String msg = "BALANCE " + pub.getEncoded(true).toString() + " " + signature;
                                Datagram d = new Datagram(0, msg);
                                d.send(s);

                                boolean b11_2 = s.isClosed();
                                boolean b12_2 = s.isConnected();

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
