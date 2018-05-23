package com.example.usgov;

import android.support.design.widget.FloatingActionButton;
import android.support.design.widget.Snackbar;
import android.util.Log;
import android.view.View;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.util.Arrays;

public class Datagram {

    static final int h=6;
    static final int maxsize=100000;

    byte[] bytes;
    int dend;
    int error;
    int service;

    Datagram() {
        bytes=new byte[h];
        dend=0;
        error=0;
    }
    Datagram(int service) {
        int size=6;
        bytes= new byte[size];
        encode_size(size);
        encode_service(service);
        dend=bytes.length;
    }
    Datagram(int service, String msg) {
        int size=6+msg.length();
        bytes= new byte[size];
        encode_size(size);
        encode_service(service);
        System.arraycopy(msg.getBytes(), 0, bytes, 6, msg.length());
        dend=bytes.length;
    }

    void encode_size(int sz) {
        assert h==6;
        assert(sz>=h);
        bytes[0]=(byte)(sz&0xff);
        bytes[1]=(byte)(sz>>8&0xff);
        bytes[2]=(byte)(sz>>16&0xff);
        bytes[3]=(byte)(sz>>24&0xff);
    }
    int decode_size() {
        assert bytes.length>3;
        int sz=bytes[0];
        sz|=bytes[1]<<8;
        sz|=bytes[2]<<16;
        sz|=bytes[3]<<24;
        return sz;
    }
    void encode_service(int svc) {
        assert h==6;
        assert(bytes.length>=h);
        bytes[4]=(byte)(svc&0xff);
        bytes[5]=(byte)(svc>>8&0xff);
    }
    int decode_service() {
        assert bytes.length>5;
        int svc=bytes[4];
        svc|=bytes[5]<<8;
        return svc;
    }

    public void send(Socket s) {
    //OutputStream out = s.getOutputStream();
/*
			datagram* q=new datagram(protocol::wallet::balance_query,"");

			string addr="localhost"; //"92.51.240.61"; //"127.0.0.1";
			uint16_t port=16673;
			socket::datagram* response=socket::peer_t::send_recv(addr,port,q);
			if (response) {
			cout << response->parse_string() << endl;
			delete response;
			}
			else {
				cerr << "ERROR" << endl;
			}



 */
    //PrintWriter output = new PrintWriter(out);
    //output.println(msg);
    //output.flush();

try {
    DataOutputStream dOut = new DataOutputStream(s.getOutputStream());
    //dOut.writeInt(message.length); // write length of the message
    dOut.write(bytes);
    //dOut.close();
}
catch(Exception e) {

}
}

/*
    bool recv(int sock, int timeout_seconds) {
        struct timeval tv;
        tv.tv_sec = timeout_seconds;
        tv.tv_usec = 0;
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
        return recv(sock);
    }
*/
    boolean recv(Socket s) {
        Log.i("usgov","recv");
        try {
//cout << "datagram::recv " << dend << " " << h << " " << size() << endl;
        //BufferedReader input = new BufferedReader(new InputStreamReader(s.getInputStream()));
        DataInputStream input = new DataInputStream(s.getInputStream());
            Log.i("usgov","dend "+String.valueOf(dend));

        if (dend < h) {
//cout << "A" << endl;
            //cout << "socket: datagram: recv1" << endl;
            Log.i("usgov","H");

            int nread = input.read(bytes, dend, h - dend);

            Log.i("usgov","nread "+String.valueOf(nread));


            //ssize_t nread = server::os->recv(sock, &(*this)[dend], h-dend, 0);
            if (nread <= 0) {
                error = nread == 0 ? 1 : 2;
                Log.i("usgov","FALSE");
                return false;
            }
            dend += nread;
//cout << "nread " << nread  << endl;
            if (dend < h) return true;
            int sz = decode_size();
//cout << "sz " << sz << endl;
            if (sz > maxsize) {
                error = 3;
                Log.i("usgov","FALSE");
                return false;
            }
            byte[] m = new byte[sz];
            System.arraycopy(bytes, 0, m, 0, h);
            bytes = m;
            service = decode_service();
            if (dend == sz) {
                Log.i("usgov","TRUE");
                return true;
            }
        }
        Log.i("usgov","body ");
//cout << "datagram::recv 2 - " << dend << " " << size() << endl;
        //cout << "socket: datagram: recv2" << endl;
        int nread = input.read(bytes, dend, bytes.length - dend);
        //ssize_t nread = server::os->recv(sock, &(*this)[dend], size()-dend,0);
        Log.i("usgov","nread "+String.valueOf(nread));
        if (nread <= 0) {
            error = nread == 0 ? 1 : 2;
//		error=nread==0?1:2;
            Log.i("usgov","FALSE");
            return false;
        }
        dend += nread;
        Log.i("usgov","dend "+String.valueOf(dend));
        Log.i("usgov","TRUE");
        return true;


    }
    catch(Exception e) {
        Log.i("usgov","Except "+e.getMessage());
        Log.i("usgov","FALSE");
        return false;
    }
    }



    String parse_string() {
        byte[] b=new byte[dend-h];
        System.arraycopy(bytes, h, b, 0, dend-h);

        return new String(b);
    }


};