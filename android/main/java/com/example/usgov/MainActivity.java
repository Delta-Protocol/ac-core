package com.example.usgov;

import us.wallet.*;

import java.io.UnsupportedEncodingException;
import java.util.Timer;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Intent;
import android.content.res.Configuration;
import android.media.MediaPlayer;

import android.media.RingtoneManager;
import android.media.Ringtone;
import android.net.Uri;
import android.os.Build;
import android.content.Context;
import android.os.Bundle;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.View;
import android.view.KeyEvent;
import android.view.Menu;
import android.view.MenuItem;
import android.view.ViewGroup;
import android.widget.Button;

import android.widget.LinearLayout;
import android.widget.Toast;

import java.io.IOException;

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



public class MainActivity extends AppCompatActivity implements AppListener {

    app a;

/*
    public void sendMessage(View view) {
        //EditText editText = (EditText) findViewById(R.id.editText);
        //String message = editText.getText().toString();
        //intent.putExtra(EXTRA_MESSAGE, "");
    }
*/

    boolean bounded;
    HostCardEmulatorService svc;

    private Button scanButton;
    private Button pay;
    private ImageView qrcode;
    private EditText amount;
    private Button balance;
    private Button newaddress;
    private TextView action;
    private pl.droidsonroids.gif.GifImageView carlton;
    private pl.droidsonroids.gif.GifImageView wait;
    private LinearLayout acquire_addr;
    private ImageView share;
    private ImageView walletdconnect;


    private final String balance_button_text="check balance";

    SmartCardReader reader;


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
    public void on_wallet_init_success() {
        runOnUiThread(new Thread(new Runnable() {
            public void run() {
                Log.d("AAAA","AAA1");
                if (a.w.walletd_host().trim().isEmpty()) {
                    Log.d("AAAA","AAA2");
                    Intent intent = new Intent(MainActivity.this, node_pairing.class);
                    startActivity(intent);
                }
                Log.d("AAAA","AAA3 -->"+a.w.walletd_host()+"<--");
                publish_myaddress();
                updateControls();
                Log.d("AAAA","AAA4");
            }
        }));
    }

    @Override
    public void on_wallet_init_error(String error) {
        final String what=error;
        runOnUiThread(new Thread(new Runnable() {
            public void run() {
                Toast.makeText(MainActivity.this, what, 30000).show();
            }
        }));

    }


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        if (isAndroidRuntime()) new LinuxSecureRandom(); //Asserts /dev/urandom is ok

        setContentView(R.layout.activity_main);

        a=(app) getApplication();


        scanButton = (Button) findViewById(R.id.scan);
        pay = (Button) findViewById(R.id.pay);
        qrcode = (ImageView) findViewById(R.id.qrCode);
        amount = (EditText) findViewById(R.id.amount);
        balance = (Button) findViewById(R.id.balance);
        action = (TextView) findViewById(R.id.action);
        acquire_addr =  (LinearLayout) findViewById(R.id.acquire_addr);
        newaddress= (Button) findViewById(R.id.newaddress);
        carlton=(pl.droidsonroids.gif.GifImageView)findViewById(R.id.carlton);
        wait=(pl.droidsonroids.gif.GifImageView)findViewById(R.id.wait);

        share=(ImageView)findViewById(R.id.share);
        walletdconnect=(ImageView)findViewById(R.id.walletdconnect);


//        Toolbar toolbar = (Toolbar) findViewById(R.id.toolbar);
//        setSupportActionBar(toolbar);

  //      FloatingActionButton fab = (FloatingActionButton) findViewById(R.id.fab);

        //pub = new LazyECPoint(curve.getCurve(), pubParams.getQ().getEncoded(true));


        if(savedInstanceState != null) {
            balance.setText(savedInstanceState.getString("balancelbl"));
        }
        else {
            balance.setText(balance_button_text);
        }
        balance.setOnClickListener(new View.OnClickListener() {
           @Override
           public void onClick(View view) {
               if (a.w==null) {
                   Toast.makeText(MainActivity.this, "Wallet is not OK", 6000).show();
                   return;
               }
               if (balance.getText() == balance_button_text) {
                   balance.setText("...");
                   //final Handler handler = new Handler();
                   Thread thread = new Thread(new Runnable() {
                       @Override
                       public void run() {
                           final String b = a.w.balance(false);
                           setBalance(b);
                           /*
                           Log.d("ZZZZZZZZZZZZ",b);
                           handler.post(new Runnable() {
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

        walletdconnect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(MainActivity.this, node_pairing.class);
                startActivity(intent);
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
                if (a.w==null) {
                    Toast.makeText(MainActivity.this, "Wallet is not OK", 6000).show();
                    return;
                }
                newaddress.setEnabled(false);
                Thread thread = new Thread(new Runnable() {
                    @Override
                    public void run() {
                        try {
                            if (a.w.renew_address()) {
                                onnewaddress();
                            }
                            else {
                                onFailNewAddress("Not an address");
                            }
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
                ClipData clip = ClipData.newPlainText("address", a.w.my_address);
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

                Toast.makeText(MainActivity.this, "My address "+a.w.my_address+" has been copied to the clipboard.", 3000).show();
            }
        });

        scanButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                scanButton.setEnabled(false);

                // start scanning
                IntentIntegrator intentIntegrator = new IntentIntegrator(MainActivity.this);
                intentIntegrator.initiateScan();

            }
        });

        carlton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                carlton.setVisibility(View.INVISIBLE);
                updateControls();

            }
        });
        wait.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                wait.setVisibility(View.INVISIBLE);
                updateControls();

            }
        });


        updateControls();

        if (a.w==null)
            on_wallet_init_error("");
        else
            on_wallet_init_success();
        a.ml=this;


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
                Toast.makeText(MainActivity.this, "My new address is "+a.w.my_address+".", 3000).show();
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
    void hidegifs() {
        runOnUiThread(new Thread(new Runnable() {
            public void run() {
                carlton.setVisibility(View.INVISIBLE);
                wait.setVisibility(View.INVISIBLE);
                updateControls();
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


    void vibrate() {
        Vibrator v = (Vibrator) getSystemService(Context.VIBRATOR_SERVICE);
        if (!v.hasVibrator()) return;
        // Vibrate for 500 milliseconds
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            v.vibrate(VibrationEffect.createOneShot(500,VibrationEffect.DEFAULT_AMPLITUDE));
        }else{
            //deprecated in API 26
            v.vibrate(500);
        }
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        IntentResult scanResult = IntentIntegrator.parseActivityResult(requestCode, resultCode, data);
        if (scanResult != null) {
            Log.i("SCAN", "scan result: " + scanResult);
            String c=scanResult.getContents();
            if (c!=null) {
                dopay(c);
            }
        } else {
            Toast.makeText(MainActivity.this, "Sorry, the scan was unsuccessful", 1000).show();
            Log.e("SCAN", "Sorry, the scan was unsuccessful...");
        }
    }

    void deliver_address_from_NFC(String rcpt_address) {
        dopay(rcpt_address);
    }


    boolean dopay(final String rcpt_address) {
        int am = 0;
        try {
            am = Integer.parseInt(amount.getText().toString());
        } catch (NumberFormatException e) {
            Toast.makeText(MainActivity.this, "Invalid amount"+amount.getText().toString(), 6000).show();
            return false;
        }
        return dopay(rcpt_address,am,1);
    }

    boolean dopay(final String rcpt_address, final int amount, final int fee) {
        if (!isValidAddress(rcpt_address)) {
            Toast.makeText(MainActivity.this, "Invalid address "+rcpt_address, 6000).show();
            return false;
        }
        if (a.w==null) {
            Toast.makeText(MainActivity.this, "Wallet is not OK", 6000).show();
            return false;
        }
        vibrate();
        hidegifs();
        showgif(wait);

        Thread thread = new Thread(new Runnable() {
            @Override
            public void run() {
                String tx=a.w.pay(amount,fee,rcpt_address);
                onTxCompleted(tx);
            }
        });

        thread.start();


        return true;
    }

    void showgif(View v) {
        transition_state(State.NONE);
        pay.setVisibility(View.INVISIBLE);
        v.setVisibility(View.VISIBLE);
        ViewGroup.LayoutParams params=v.getLayoutParams();
        params.width=MainActivity.this.findViewById(android.R.id.content).getWidth();
        params.height=MainActivity.this.findViewById(android.R.id.content).getHeight();
        v.setLayoutParams(params);
        v.bringToFront();

    }

    boolean isTxValid(String tx) {
        Log.d("B58","verifying Tx: "+tx);
        try {
            byte[] decoded=Base58.decode(tx);
            Log.d("B58","decoded:"+new String(decoded,"UTF-8"));
            return decoded.length>0;
        }
        catch(Base58.AddressFormatException e) {
            Log.d("B58","NOPES");
            return false;
        }
        catch(UnsupportedEncodingException e) {
            Log.d("B58","NOPES wrong UTF8 encoding");
            return false;
        }
    }

    void onTxCompleted(final String tx) {
        Log.d("CASH","Tx completed: "+tx);
        final boolean b=isTxValid(tx);
        runOnUiThread(new Thread(new Runnable() {
            public void run() {
                if (!b) {
                    try {
                        MediaPlayer mp = MediaPlayer.create(getApplicationContext(), R.raw.error);
                        mp.start();
                    } catch (Exception e) {
                        //   e.printStackTrace();
                    }
                    hidegifs();
                    Toast.makeText(MainActivity.this, "Received an indecipherable transaction  : "+tx, Toast.LENGTH_LONG).show();
                    return;
                }
                //Toast.makeText(MainActivity.this, "transaction is: "+tx, 6000).show();
                Toast.makeText(MainActivity.this, "transaction sent successfully :)", Toast.LENGTH_LONG).show();
                try {
                    Uri notification = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_NOTIFICATION);
                    Ringtone r = RingtoneManager.getRingtone(getApplicationContext(), notification);
                    r.play();
                } catch (Exception e) {
                 //   e.printStackTrace();
                }
                hidegifs();
                showgif(carlton);

                //qrcode.g setContentView(new gifView(MainActivity.this) );
            }
        }));

        Timer timer=new Timer();
        timer.schedule(new TimerTask() {
            @Override
            public void run() {
                hidegifs();
            }
        }, 5*1000);

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
        if (a.w==null) {
            qrcode.setVisibility(View.INVISIBLE);
        }
        else {
            qrcode.setVisibility(r ? View.INVISIBLE : View.VISIBLE);
        }
        pay.setVisibility(View.VISIBLE);

        action.setVisibility(r ? View.VISIBLE : View.INVISIBLE);
        acquire_addr.setVisibility(r ? View.VISIBLE : View.INVISIBLE);
        newaddress.setVisibility(View.INVISIBLE);
        scanButton.setEnabled(true);

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
        if (a.w==null) {
            return;
        }
        MultiFormatWriter multiFormatWriter = new MultiFormatWriter();
        try {
            String txt=a.w.my_address;
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
        if (a.w==null) {
            return;
        }
        sendDataToNFCService(a.w.my_address);
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
