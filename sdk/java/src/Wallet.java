package us.wallet;

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
import org.spongycastle.math.ec.ECPoint;
import java.security.GeneralSecurityException;

public class Wallet {

    private BigInteger priv;
    private ECPoint pub;
    public String my_address;

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


    private File homeDir;

    public Wallet(File homedir) throws IOException, GeneralSecurityException {
	homeDir=homedir;
        setup_keys();
        setup_addr();
        setup_walletd_host();
        setup_walletd_port();
    }



    public FileOutputStream getOutputStream(String filename) throws IOException {
	//override. i.e. in Android: return ctx.openFileOutput(filename, Context.MODE_PRIVATE);
	return new FileOutputStream(filename);
    }

    void setup_keys() throws IOException, GeneralSecurityException {
        String filename = "k";
        String fileContents;
        File file = new File(homeDir,filename);
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
//            outputStream = ctx.openFileOutput(filename, Context.MODE_PRIVATE);
            outputStream = getOutputStream(filename);
            outputStream.write(fileContents.getBytes());
            outputStream.write('\n');
            outputStream.close();
        }
        else {

            fileContents = getStringFromFile(file);


        }

        try {
            priv = new BigInteger(fileContents);
        }
        catch(Exception e){
           // Log.e("Wallet",e.getMessage());
            throw new IOException("Cannotmake priv key");
        }
        if(priv==null){
            throw new IOException("priv key is null");
            //Log.d("Wallet", "Private key was not successfully retrieved");
        }
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

    void setup_addr() throws IOException  {
        String filename = "a";
        File file = new File(homeDir,filename);
        if(!file.exists()) {
            renew_address();
        }
        my_address=getStringFromFile(file);
    }

    void setup_walletd_host() throws IOException  {
        String filename = "n";
        File file = new File(homeDir,filename);
        if(file.exists()) {
	        walletdAddress=getStringFromFile(file);
//            throw new IOException("Walletd-endpoint file does not exist. "+homeDir+"/n");
        }
	else {
	        walletdAddress="127.0.0.1";
	}
    }
    void setup_walletd_port() throws IOException  {
        walletdPort=16673;
        String filename = "p";
        File file = new File(homeDir,filename);
        if(file.exists()) {
		try {
		        walletdPort=Integer.parseInt(getStringFromFile(file));
		}
		catch(NumberFormatException E) {
		}
        }
    }

    public void set_walletd_host(String addr) throws IOException {
        String filename = "n";
        File file = new File(homeDir,filename);
        file.getParentFile().mkdirs();
        file.createNewFile();
        FileOutputStream outputStream;
        outputStream = getOutputStream(filename);
        outputStream.write(addr.getBytes());
        outputStream.write('\n');
        outputStream.close();
        walletdAddress=getStringFromFile(file);
    }

    public boolean set_walletd_port(int port) throws IOException {
        String filename = "p";
        File file = new File(homeDir,filename);
        file.getParentFile().mkdirs();
        file.createNewFile();
        FileOutputStream outputStream;
        outputStream = getOutputStream(filename);
        outputStream.write(Integer.toString(port).getBytes());
        outputStream.write('\n');
        outputStream.close();
	try {
        	walletdPort=Integer.parseInt(getStringFromFile(file));
		return true;
	}
	catch(NumberFormatException e) {
		return false;
	}
    }

    public String getStringFromFile (File fl) {
        try {
            FileInputStream fin = new FileInputStream(fl);
            String ret = convertStreamToString(fin);
            fin.close();
            return ret;
        }
        catch(Exception e) {
            return "";
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
    private int walletdPort=16673;

    public String walletd_host() {
        return walletdAddress;
    }

    public int walletd_port() {
        return walletdPort;
    }

    public String pay(int amount, int fee, String rcpt_address) {
        tx_make_p2pkh_input i=new tx_make_p2pkh_input(rcpt_address,amount,fee,tx_make_p2pkh_input.sigcode_all,tx_make_p2pkh_input.sigcode_all,true);
        if (!i.check()) return "Error: Invalid input data";

        String args=i.to_string();
        //Log.d("Wallet","Pay order "+args);

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
            Socket s = new Socket(walletd_host(), walletd_port());
            d.send(s);
            Datagram r=null;
            while (true) {
                r=complete_datagram(s);
                if (r==null) break;
                if (r.completed()) break;
            }
            s.close();
            return r;
        } catch (IOException e) {
        }

        return null;
    }

    String ask(short service, String args) {
        Datagram d=new Datagram(service,args);
        Datagram r=send_recv(d);
        if (r==null) return "?";
        String st;
        st = r.parse_string();
        //Log.d("Wallet","ans "+st);
        return st.trim();
    }

    public boolean isAddressValid(String addr) {
        try {
            byte[] decoded=Base58.decode(addr);
            return decoded.length>0;
        }
        catch(Base58.AddressFormatException e) {
            return false;
        }
    }

    public boolean renew_address() throws IOException {
        String addr=new_address();
        if (!isAddressValid(addr)) {
            return false;
        }
        my_address=addr;

        String filename = "a";
        File file = new File(homeDir,filename);
        file.getParentFile().mkdirs();
        file.createNewFile();
        FileOutputStream outputStream;
        outputStream = getOutputStream(filename);
        outputStream.write(my_address.getBytes());
        outputStream.write('\n');
        outputStream.close();
        return true;
    }

    public String balance(boolean detailed) {
        return ask(protocol_balance_query,detailed?"1":"0");
    }

    public String new_address() {
        return ask(protocol_new_address_query,"");
    }


}
