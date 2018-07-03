package com.example.usgov;

import android.content.Context;
import android.util.Log;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.math.BigInteger;
import java.net.Socket;
import java.security.SecureRandom;
import android.content.ContextWrapper;
import org.spongycastle.math.ec.ECPoint; 

public class Wallet {

    private BigInteger priv;
    private ECPoint pub;
    public String my_address;
    private Context ctx;

    public class tx_make_p2pkh_input {
        public static final int sigcode_all=0;
        public static final int sigcode_none=1;
        public static final int sigcode_this=2;

        public tx_make_p2pkh_input(String rcpt_addr0, int amount0, int fee0, int sigcode_inputs0, int sigcode_outputs0, boolean sendover0) {
            rcpt_addr=rcpt_addr0;
            amount=amount0;
            fee=fee0;
            if (amount<1) amount=0;
            if (fee<1) fee=0;
            sigcode_inputs=sigcode_inputs0;
            sigcode_outputs=sigcode_outputs0;
            sendover=sendover0;
        }

        String rcpt_addr;
        int amount;
        int fee;
        int sigcode_inputs;
        int sigcode_outputs;
        boolean sendover;

        boolean check() {
            return fee>0 && amount>0 && !rcpt_addr.isEmpty();
        }

        String to_string() {
            return rcpt_addr+" "+amount+" "+fee+" "+sigcode_all+" "+sigcode_all+" "+(sendover?"1":"0");
        }
    }

    void setup_keys() throws IOException {
        String filename = "k";
        String fileContents;
        File file = new File(ctx.getFilesDir(),filename);
        if(!file.exists()) {
            file.getParentFile().mkdirs();
    //        try {
                file.createNewFile();
      //      }
        //    catch(Exception e) {
          //  }
            //           log+=";new file";
//            if (!file.mkdirs()) {
            //              //Log.e(LOG_TAG, "Directory not created");
            //            log+=";Directory not created";
            //      }

            fileContents = EllipticCryptography.getInstance().generatePrivateKey().toString();
            //log+=";"+fileContents.length();
            FileOutputStream outputStream;

           // try {
                outputStream = ctx.openFileOutput(filename, Context.MODE_PRIVATE);
                outputStream.write(fileContents.getBytes());
                outputStream.write('\n');
                outputStream.close();
          //  } catch (Exception e) {
 //               Log.d("Wallet",e.getMessage());
          //  }

        }
        else {

            fileContents = getStringFromFile(file);


        }
        priv = new BigInteger(fileContents);
        pub = EllipticCryptography.getInstance().publicPointFromPrivate(priv);
    }  

    //must be in sync with the c++ master file wallet/protocol.h
    public static final short wallet_base = 0;
    public static final short protocol_balance_query = wallet_base+1;
    public static final short protocol_dump_query = wallet_base+2;
    public static final short protocol_new_address_query = wallet_base+3;
    public static final short protocol_add_address_query = wallet_base+4;
    public static final short protocol_tx_make_p2pkh_query = wallet_base+5;
    public static final short protocol_tx_sign_query = wallet_base+6;
    public static final short protocol_tx_send_query = wallet_base+7;
    public static final short protocol_tx_decode_query = wallet_base+8;
    public static final short protocol_tx_check_query = wallet_base+9;
    public static final short protocol_pair_query = wallet_base+10;
    public static final short protocol_unpair_query = wallet_base+11;
    public static final short protocol_list_devices_query = wallet_base+12;

    public static final short protocol_response = wallet_base+0;

    

    public Wallet(Context ctx_) throws IOException {
        ctx=ctx_;
        setup_keys();
        setup_addr();
        setup_walletd_host();
    }

    void setup_addr() throws IOException  {
        String filename = "a";
        File file = new File(ctx.getFilesDir(),filename);
        if(!file.exists()) {
            renew_address();
        }
        my_address=getStringFromFile(file);
    }

    void setup_walletd_host() throws IOException  {
        String filename = "n";
        File file = new File(ctx.getFilesDir(),filename);
        if(!file.exists()) {
            Log.d("Wallet","Walletd address file does not exist.");
            return;
        }
        walletdAddress=getStringFromFile(file);
        Log.d("Wallet","Read walletd address from file."+walletdAddress);
    }

    void set_walletd_host(String addr) throws IOException {
        String filename = "n";
        File file = new File(ctx.getFilesDir(),filename);
        file.getParentFile().mkdirs();
        file.createNewFile();
        FileOutputStream outputStream;
        outputStream = ctx.openFileOutput(filename, Context.MODE_PRIVATE);
        outputStream.write(addr.getBytes());
        outputStream.write('\n');
        outputStream.close();
        walletdAddress=getStringFromFile(file);
    }

    public String getStringFromFile (File fl) {
        try {
            FileInputStream fin = new FileInputStream(fl);
            String ret = convertStreamToString(fin);
            //Make sure you close all streams.
            fin.close();
            return ret;
        }
        catch(Exception e) {
            return "";//e.getStackTrace().toString();
        }
    }

    public String convertStreamToString(InputStream is) throws Exception { //1 line
        BufferedReader reader = new BufferedReader(new InputStreamReader(is));

        StringBuilder sb = new StringBuilder();
        String line = null;
        while ((line = reader.readLine()) != null) {
            sb.append(line);
            break;
        }
        reader.close();
        return sb.toString();
    }

    private String walletdAddress="";

    String walletd_host() {
        return walletdAddress;
    }

    int walletd_port() {
        return 16673;
    }

    String pay(int amount, int fee, String rcpt_address) {
        tx_make_p2pkh_input i=new tx_make_p2pkh_input(rcpt_address,amount,fee,tx_make_p2pkh_input.sigcode_all,tx_make_p2pkh_input.sigcode_all,true);
        if (!i.check()) return "Error: Invalid input data";

        String args=i.to_string();
        Log.d("Wallet","Pay order "+args);

        return ask(protocol_tx_make_p2pkh_query,args);
    }

    private Datagram curd=null;

    private Datagram complete_datagram(Socket sock) {
        if (curd==null) curd=new Datagram();
        if (!curd.recv(sock)) {
            curd=null;
            return null;
        }
        if (curd.completed()) {
            Datagram t=curd;
            curd=null;
            return t;
        }
        return curd;
    }

    public synchronized Datagram send_recv(Datagram d) {
        try {
            Log.d("Wallet","Connecting to "+walletd_host()+":"+walletd_port());
            Socket s = new Socket(walletd_host(), walletd_port());
            Log.d("Wallet","before send - isClosed "+s.isClosed());
            Log.d("Wallet","before send - isConnected "+s.isConnected());
            Log.d("Wallet","about to send datagram - length "+d.bytes.length);
            Log.d("Wallet","about to send datagram - str "+d.parse_string());

            d.send(s);
            Log.d("Wallet","after send - isClosed "+s.isClosed());
            Log.d("Wallet","after send - isConnected "+s.isConnected());

            Datagram r=null;
            while (true) {
                r=complete_datagram(s);
                if (r==null) break;
                if (r.completed()) break;
                Log.d("Wallet", "waiting for datagram " + r.bytes.length);
            }
            if (r==null) {
                Log.d("Wallet", "Received null datagram ");
            }
            else {
                Log.d("Wallet", "Received datagram " + r.bytes.length);
            }
            s.close();
            return r;
        } catch (IOException e) {
            Log.d("Datagram","exception: "+e.getMessage());
            //e.printStackTrace();
        }

        return null;
    }

    String ask(short service, String args) {
        Log.d("Wallet","ask "+service+" " +args);
        Datagram d=new Datagram(service,args);
        Datagram r=send_recv(d);
        if (r==null) return "?";
        String st;
        st = r.parse_string();
        Log.d("Wallet","ans "+st);
        return st.trim();
    }
    boolean isAddressValid(String addr) {
        try {
            byte[] decoded=Base58.decode(addr);
            return decoded.length>0;
        }
        catch(Base58.AddressFormatException e) {
            return false;
        }
    }

    boolean renew_address()throws IOException {
        String addr=new_address();
        if (!isAddressValid(addr)) {
            return false;
        }
        my_address=addr;

        String filename = "a";
        File file = new File(ctx.getFilesDir(),filename);
        file.getParentFile().mkdirs();
        file.createNewFile();
        FileOutputStream outputStream;
        outputStream = ctx.openFileOutput(filename, Context.MODE_PRIVATE);
        outputStream.write(my_address.getBytes());
        outputStream.write('\n');
        outputStream.close();
        return true;
    }

    String balance(boolean detailed) {
        return ask(protocol_balance_query,detailed?"1":"0");
    }
    String new_address() {
        return ask(protocol_new_address_query,"");
    }


}
