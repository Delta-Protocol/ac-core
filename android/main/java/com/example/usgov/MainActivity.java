package com.example.usgov;

import java.io.FileNotFoundException;
import java.util.Timer;
import android.content.ClipData;
import android.content.ClipboardManager;
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

import android.widget.LinearLayout;
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
import java.util.TimerTask;

import android.view.inputmethod.InputMethodManager;
import android.app.Activity;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.ImageView;

import com.google.zxing.BarcodeFormat;
import com.google.zxing.MultiFormatWriter;
import com.google.zxing.WriterException;
import com.google.zxing.common.BitMatrix;
import android.graphics.Bitmap;

//import com.google.zxing.integration.android.IntentIntegrator;
import com.journeyapps.barcodescanner.BarcodeEncoder;



public class MainActivity extends AppCompatActivity {
    boolean bounded;
    HostCardEmulatorService svc;

    private Button scanButton;
    private Button pay;
    private ImageView qrcode;
    private EditText amount;
    private Button balance;
    private Button newaddress;
    private TextView action;
//    private ImageView nfc_logo;
    private LinearLayout acquire_addr;

    private final String balance_button_text="check balance";

    SmartCardReader reader;

    Wallet w;

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
        updateControls();
    }

    public static boolean isAndroidRuntime() {
            final String runtime = System.getProperty("java.runtime.name");
            return runtime != null && runtime.equals("Android Runtime");
    }


    public enum State {
        NONE, CHARGE, PAY
    }

    State state=State.NONE;

    String getVerb() {
        switch (state) {
            case NONE:
                return "NONE";
            case CHARGE:
                return "CHARGE";
            case PAY:
                return "PAY";
        }
        return "";
    }

    void transition_state(State st) {
        if (st==state) return;
        state=st;
        switch (state) {
            case NONE:
                //reader.no_intent();
                break;
            case CHARGE: {
                //EditText a = (EditText) findViewById(R.id.amount);
                //reader.pay_intent("Charge", a.getText().toString());
            }
                break;
            case PAY: {
                //EditText a = (EditText) findViewById(R.id.amount);
                //reader.pay_intent("Pay", a.getText().toString());
            }
            break;

        }
        updateControls();
    }


    @Override
    protected void onSaveInstanceState(Bundle astate) {
        super.onSaveInstanceState(astate);
        astate.putString("balancelbl", balance.getText().toString());
        astate.putInt("state", state.ordinal());
        EditText a = (EditText) findViewById(R.id.amount);
        astate.putString("amount", a.getText().toString());

    }


    void setBalance(String b) {
        final String B=b;
        Log.d("ZZZZZZZZZxxZ",B);
        runOnUiThread(new Thread(new Runnable() {
            public void run() {
                balance.setText(cash_human.show(B));
            }
        }));


    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (isAndroidRuntime()) new LinuxSecureRandom(); //Asserts /dev/urandom is ok

        setContentView(R.layout.activity_main);

        scanButton = (Button) findViewById(R.id.scan);
        pay = (Button) findViewById(R.id.pay);
        qrcode = (ImageView) findViewById(R.id.qrCode);
        amount = (EditText) findViewById(R.id.amount);
        balance = (Button) findViewById(R.id.balance);
        action = (TextView) findViewById(R.id.action);
        acquire_addr =  (LinearLayout) findViewById(R.id.acquire_addr);
        newaddress= (Button) findViewById(R.id.newaddress);

//        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
//        setSupportActionBar(toolbar);

  //      FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);

        //pub = new LazyECPoint(curve.getCurve(), pubParams.getQ().getEncoded(true));

        initWallet();

        if(savedInstanceState != null) {
            balance.setText(savedInstanceState.getString("balancelbl"));
        }
        else {
            balance.setText(balance_button_text);
        }
        balance.setOnClickListener(new View.OnClickListener() {
           @Override
           public void onClick(View view) {
               if (w==null) {
                   Toast.makeText(MainActivity.this, "Wallet is not OK", 6000).show();
                   return;
               }
               if (balance.getText() == balance_button_text) {
                   balance.setText("...");
                   //final Handler handler = new Handler();
                   Thread thread = new Thread(new Runnable() {
                       @Override
                       public void run() {
                           final String b = w.balance(false);
                           setBalance(b);
dopay(w.my_address);
                           /*
                           Log.d("ZZZZZZZZZZZZ",b);
                           handler.post(new Runnable() {
                               @Override
                               public void run() {

                               }
                           });
                           */
                       }
                   });

                   thread.start();

               } else {
                   balance.setText(balance_button_text);
               }

           }
       }
       );



/*
        fab.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Snackbar.make(view, flog, Snackbar.LENGTH_LONG)
                        .setAction("Action", null).show();
            }
        });
*/
        pay.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (state==State.NONE) {
                    transition_state(State.PAY);
                }
                else { //Cancel
                    transition_state(State.NONE);
                }
                Log.d("XXX","calling UpdateControls from pay onclick");
//                updateControls();
            }
        });
/*
        Button get_paid = (Button) findViewById(R.id.get_paid);
        get_paid.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Log.d("XXX","calling UpdateControls from getpaid onclick");
                transition_state(State.CHARGE);

            }
        });
*/
        amount.setOnFocusChangeListener(new View.OnFocusChangeListener() {
            @Override
            public void onFocusChange(View v, boolean hasFocus) {
                if (!hasFocus) {
                    hideKeyboard(v);
                }
            }
        });

  //      findViewById(R.id.get_paid).setEnabled(false);
        pay.setEnabled(false);

        amount.setOnKeyListener(new View.OnKeyListener() {
            @Override

            public boolean onKey(View v, int keyCode, KeyEvent event) {
                if (amount.getText().length()==0) {
//                    findViewById(R.id.get_paid).setEnabled(false);
                    pay.setEnabled(false);

                } else {
  //                  findViewById(R.id.get_paid).setEnabled(true);
                    pay.setEnabled(true);
                }
                return false;
            }
        });


        reader=new SmartCardReader(this);

        if(savedInstanceState != null) {
            balance.setText(savedInstanceState.getString("balancelbl"));
            transition_state(State.values()[savedInstanceState.getInt("state")]);
        }
        else {
            balance.setText(balance_button_text);
            transition_state(State.NONE);
        }
        Log.d("XXX","calling UpdateControls from onCreate");



        newaddress.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                if (w==null) {
                    Toast.makeText(MainActivity.this, "Wallet is not OK", 6000).show();
                    return;
                }
                newaddress.setEnabled(false);
                Thread thread = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            w.renew_address();
                            onnewaddress();
                        }
                        catch(IOException e) {
                            onFailNewAddress(e.getMessage());
                        }

                           /*
                           Log.d("ZZZZZZZZZZZZ",b);
                           handler.post(new Runnable() {
                               @Override
                               public void run() {

                               }
                           });
                           */
                    }
                });

                thread.start();



            }
        });


        qrcode.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                ClipboardManager clipboard = (ClipboardManager) getSystemService(Context.CLIPBOARD_SERVICE);
                ClipData clip = ClipData.newPlainText("address", w.my_address);
                clipboard.setPrimaryClip(clip);
                newaddress.setEnabled(true);
                newaddress.setVisibility(View.VISIBLE);
                Timer timer=new Timer();
                timer.schedule(new TimerTask() {
                    @Override
                    public void run() {
                        hidenewaddr();
                    }
                }, 2*1000);

                Toast.makeText(MainActivity.this, "My address "+w.my_address+" has been copied to the clipboard.", 3000).show();
            }
        });

        scanButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                // start scanning
                IntentIntegrator intentIntegrator = new IntentIntegrator(MainActivity.this);
                intentIntegrator.initiateScan();

            }
        });

        updateControls();
    }

    void onFailNewAddress(final String error) {
        runOnUiThread(new Thread(new Runnable() {
            public void run() {
                newaddress.setVisibility(View.INVISIBLE);
                updateControls();
                Toast.makeText(MainActivity.this, error, 30000).show();
            }
        }));
    }

    void onnewaddress() {
        publish_myaddress();
        runOnUiThread(new Thread(new Runnable() {
            public void run() {
                newaddress.setVisibility(View.INVISIBLE);
                updateControls();
                Toast.makeText(MainActivity.this, "My new address is "+w.my_address+".", 3000).show();
            }
        }));
    }

    void hidenewaddr() {
        runOnUiThread(new Thread(new Runnable() {
            public void run() {
                if (newaddress.isEnabled()) //dont do anything until the new address arrives
                    newaddress.setVisibility(View.INVISIBLE);
            }
        }));
    }

    public void sendDataToNFCService(String data){
        Context context = getApplicationContext();
        Intent intent = new Intent(context, HostCardEmulatorService.class);
        intent.putExtra("message",data);
        context.startService(intent);
    }

    boolean isValidAddress(String addr) {
        return true;
    }

    void WalletInitError(final String what) {
        runOnUiThread(new Thread(new Runnable() {
            public void run() {
                Toast.makeText(MainActivity.this, what, 30000).show();
            }
        }));
    }
    void onWalletInitSuccess() {
        runOnUiThread(new Thread(new Runnable() {
            public void run() {
                publish_myaddress();
                updateControls();
            }
        }));
    }

    void initWallet() {
        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                try {
                    w = new Wallet(MainActivity.this);
                    onWalletInitSuccess();
                }
                catch(IOException e) {
                    WalletInitError(e.getMessage());
                }
            }
        });

        thread.start();


    }


    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        IntentResult scanResult = IntentIntegrator.parseActivityResult(requestCode, resultCode, data);
        if (scanResult != null) {
            Log.i("SCAN", "scan result: " + scanResult);
            dopay(scanResult.getContents());
        } else {
            Toast.makeText(MainActivity.this, "Sorry, the scan was unsuccessful", 1000).show();
            Log.e("SCAN", "Sorry, the scan was unsuccessful...");
        }
    }

    void deliver_address_from_NFC(String rcpt_address) {
        dopay(rcpt_address);
    }


    void dopay(final String rcpt_address) {
        if (!isValidAddress(rcpt_address)) {
            Toast.makeText(MainActivity.this, "Invalid address "+rcpt_address, 6000).show();
            return;
        }
        if (w==null) {
            Toast.makeText(MainActivity.this, "Wallet is not OK", 6000).show();
            return;
        }
        final String am=amount.getText().toString();
        final String fee="1";

        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                String tx=w.pay(am,fee,rcpt_address);
                onTxCompleted(tx);
            }
        });

        thread.start();



    }

    void onTxCompleted(final String tx) {
        Log.d("CASH","Tx completed: "+tx);
        runOnUiThread(new Thread(new Runnable() {
            public void run() {
                Toast.makeText(MainActivity.this, "transaction is: "+tx, 6000).show();
            }
        }));

    }

    @Override
    protected void onStart() {
        super.onStart();
/*
        Intent mIntent = new Intent(this, HostCardEmulatorService.class);
        bindService(mIntent, mConnection, BIND_AUTO_CREATE);-
        */
    }


    @Override
    protected void onStop() {
        super.onStop();
        /*
        if(bounded) {
            unbindService(mConnection);
            bounded = false;
        }
        */
    };
/*
    ServiceConnection mConnection = new ServiceConnection() {
        @Override
        public void onServiceDisconnected(ComponentName name) {
            Toast.makeText(MainActivity.this, "Service is disconnected", 1000).show();
            bounded = false;
            svc = null;
        }

        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            //            Toast.makeText(Client.this, "Service is connected", 1000).show();
            bounded = true;
            HostCardEmulatorService.LocalBinder mLocalBinder = (HostCardEmulatorService.LocalBinder)service;
            svc = mLocalBinder.getInstance();

        }
    };
*/
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
/*
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
*/
    //String verb=new String();

    public void updateControls() {
        boolean r=state!=State.NONE;
//        boolean r=reader.is_reading();
Log.d("XXXXXXXX-UpdateControls",""+r);
        amount.setVisibility(r ? View.INVISIBLE : View.VISIBLE);
        if (w==null) {
            qrcode.setVisibility(View.INVISIBLE);
        }
        else {
            qrcode.setVisibility(r ? View.INVISIBLE : View.VISIBLE);
        }

        action.setVisibility(r ? View.VISIBLE : View.INVISIBLE);
        acquire_addr.setVisibility(r ? View.VISIBLE : View.INVISIBLE);
        newaddress.setVisibility(View.INVISIBLE);
//        findViewById(R.id.get_paid).setVisibility(r ? View.INVISIBLE : View.VISIBLE);
        if (r) {
            action.setText(getVerb()+" " +cash_human.show(amount.getText().toString()));
            pay.setText("CANCEL");
//            findViewById(R.id.get_paid).setEnabled(true);
            pay.setEnabled(true);
        }
        else {
            pay.setText("PAY");
            pay.setEnabled(amount.getText().length()!=0);
        }
        paintQR();
        switch(state) {
            case NONE:
                reader.disable();
                break;
            case PAY:
                reader.enable();
                break;
            case CHARGE:
                reader.enable();
                break;

        }

    }


    public void paintQR() {
        if (w==null) {
            return;
        }
        MultiFormatWriter multiFormatWriter = new MultiFormatWriter();
        try {
            String txt=w.my_address;
            if (txt.isEmpty()) txt="-";
            BitMatrix bitMatrix = multiFormatWriter.encode(txt, BarcodeFormat.QR_CODE, 200, 200);
            BarcodeEncoder barcodeEncoder = new BarcodeEncoder();
            Bitmap bitmap = barcodeEncoder.createBitmap(bitMatrix);
            qrcode.setImageBitmap(bitmap);
        } catch (WriterException e) {
            e.printStackTrace();
        }
    }



    void publish_myaddress() {
        if (w==null) {
            return;
        }
        sendDataToNFCService(w.my_address);
    }




    public void hideKeyboard(View view) {
        InputMethodManager inputMethodManager =(InputMethodManager)getSystemService(Activity.INPUT_METHOD_SERVICE);
        inputMethodManager.hideSoftInputFromWindow(view.getWindowToken(), 0);
    }


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
