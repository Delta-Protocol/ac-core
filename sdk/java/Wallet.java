package com.example.usgov;

import android.content.Context;
import android.util.Log;

import org.spongycastle.asn1.x9.X9ECParameters;
import org.spongycastle.crypto.AsymmetricCipherKeyPair;
import org.spongycastle.crypto.ec.CustomNamedCurves;
import org.spongycastle.crypto.generators.ECKeyPairGenerator;
import org.spongycastle.crypto.params.ECDomainParameters;
import org.spongycastle.crypto.params.ECKeyGenerationParameters;
import org.spongycastle.crypto.params.ECPrivateKeyParameters;
import org.spongycastle.crypto.params.ECPublicKeyParameters;
import org.spongycastle.math.ec.ECPoint;
import org.spongycastle.math.ec.FixedPointCombMultiplier;
import org.spongycastle.math.ec.FixedPointUtil;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.math.BigInteger;
import java.net.Socket;
import java.security.SecureRandom;
import android.content.Context;
import android.content.ContextWrapper;

public class Wallet {

    public BigInteger priv;
    public ECPoint pub;
    private static final SecureRandom secureRandom;

    public static final X9ECParameters curve_params = CustomNamedCurves.getByName("secp256k1");
    public static final ECDomainParameters curve; // The parameters of the secp256k1 curve that Bitcoin uses.
    static {
        //if (Utils.isAndroidRuntime()) new LinuxSecureRandom();
        FixedPointUtil.precompute(curve_params.getG(), 12);
        curve= new ECDomainParameters(curve_params.getCurve(), curve_params.getG(), curve_params.getN(), curve_params.getH());
        secureRandom = new SecureRandom();
    }
    Context ctx;

    //must be in sync with the c++ master file wallet/protocol.h
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
    public static final int pair_query = wallet_base+10;
    public static final int unpair_query = wallet_base+11;
    public static final int list_devices_query = wallet_base+12;

    public static final int protocol_response = wallet_base+0;


    public Wallet(Context ctx_) {
        ctx=ctx_;

        String filename = "k";


        //     log="files dir:"+getFilesDir();

        File file = new File(ctx.getFilesDir(),filename);
        if(!file.exists()) {
            file.getParentFile().mkdirs();
            try {
                file.createNewFile();
            }
            catch(Exception e) {
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
                outputStream = ctx.openFileOutput(filename, Context.MODE_PRIVATE);
                outputStream.write(fileContents.getBytes());
                outputStream.write('\n');
                outputStream.close();
            } catch (Exception e) {
                Log.d("Wallet",e.getMessage());
            }

        }
        else {

            String content = getStringFromFile(file);
            priv=new BigInteger(content);

        }
        //pub = new LazyECPoint(curve.getCurve(), pubParams.getQ().getEncoded(true));
        pub = publicPointFromPrivate(priv);
        //log=pub.getEncoded(true).toString();
        //pub=new LazyECPoint(priv);
        //toggleContactless("AA");

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

    String walletd_host() {
        return "92.51.240.61";
    }

    int walletd_port() {
        return 16673;
    }

    String pay(String amount, String rcpt_address) {
        return "";
    }

    Datagram send_recv(Datagram d) {
        try {
            Socket s = new Socket(walletd_host(),walletd_port());
            boolean b11 = s.isClosed();
            boolean b12 = s.isConnected();
            d.send(s);
            boolean b11_2 = s.isClosed();
            boolean b12_2 = s.isConnected();

            Datagram r = new Datagram();
            if (r.recv(s)) {
                s.close();
                return r;
            } else {
                s.close();
                return null;
            }
        } catch (IOException e) {
            //e.printStackTrace();
        }
        return null;
    }

    String ask(int service, String args) {
        Datagram d=new Datagram(service,args);
        Datagram r=send_recv(d);
        if (r==null) return "?";
        String st;
        st = r.parse_string();
        return st;
    }

    String balance(boolean detailed) {
        return ask(protocol_balance_query,detailed?"1":"0");
    }
}
