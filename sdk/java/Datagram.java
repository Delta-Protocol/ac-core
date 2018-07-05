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
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
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
    Datagram(short service) {
        int size=6;
        bytes= new byte[size];
        encode_size(size);
        encode_service(service);
        dend=bytes.length;
    }
    Datagram(short service, String msg) {
        int size=6+msg.length();
        bytes= new byte[size];
        encode_size(size);
        encode_service(service);
        System.arraycopy(msg.getBytes(), 0, bytes, 6, msg.length());
        dend=bytes.length;
    }

    boolean completed() {
        return dend==bytes.length && bytes.length>0;
    }

    void encode_size(int sz) {
        assert h==6;
        assert(sz>=h);
        byte[] t=new byte[4];
        ByteBuffer bb = ByteBuffer.wrap(t);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        bb.asIntBuffer().put(sz);
        bytes[0]=t[0];
        bytes[1]=t[1];
        bytes[2]=t[2];
        bytes[3]=t[3];

        /*
        bytes[0]=(byte)(sz&0xff);
        bytes[1]=(byte)(sz>>8&0xff);
        bytes[2]=(byte)(sz>>16&0xff);
        bytes[3]=(byte)(sz>>24&0xff);
        */
    }
    int decode_size() {
        assert bytes.length>3;
        byte[] t=new byte[4];
        t[0]=bytes[0];
        t[1]=bytes[1];
        t[2]=bytes[2];
        t[3]=bytes[3];
        ByteBuffer bb = ByteBuffer.wrap(t);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        return bb.getInt();
        /*
        int sz=bytes[0];
        Log.d("Datagram","decode size byte 0 = "+bytes[0]+" sz="+sz);
        sz|=bytes[1]<<8;
        Log.d("Datagram","decode size byte 1 = "+bytes[1]+" sz="+sz);
        sz|=bytes[2]<<16;
        Log.d("Datagram","decode size byte 2 = "+bytes[2]+" sz="+sz);
        sz|=bytes[3]<<24;
        Log.d("Datagram","decode size byte 3 = "+bytes[3]+" sz="+sz);
        return sz;
        */
    }
    void encode_service(short svc) {
        assert h==6;
        assert(bytes.length>=h);

        byte[] t=new byte[2];
        ByteBuffer bb = ByteBuffer.wrap(t);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        bb.asShortBuffer().put(svc);
        bytes[4+0]=t[0];
        bytes[4+1]=t[1];

//        bytes[4]=(byte)(svc&0xff);
//        bytes[5]=(byte)(svc>>8&0xff);
    }
    short decode_service() {
        assert bytes.length>5;
        byte[] t=new byte[2];
        t[0]=bytes[4+0];
        t[1]=bytes[4+1];
        ByteBuffer bb = ByteBuffer.wrap(t);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        return bb.getShort();
/*

        int svc=bytes[4];
        svc|=bytes[5]<<8;
        return svc;
*/
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
    Log.d("Datagram","Exception "+e.getMessage());
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
/*
    boolean recv(Socket s, int timeout_secs) {
        public <C extends SelectableChannel & ReadableByteChannel>byte[]
        receive(C chan) throws IOException
        {
            logger.debug(TAG + " Client Recieving...");
            try
            {
                Selector sel = Selector.open();
                SelectionKey key = chan.register(sel, SelectionKey.OP_READ);
                ByteBuffer inputData = ByteBuffer.allocate(1024);
                long timeout = TIMEOUT;
                while (inputData.hasRemaining()) {
                    if (timeout < 0L) {
                        throw new IOException(String.forma45t("Timed out, %d of %d bytes read", inputData.position(), inputData.limit()));
                    }
                    long startTime = System.nanoTime();
                    sel.select(timeout);
                    long endTime = System.nanoTime();
                    timeout -= TimeUnit.NANOSECONDS.toMillis(endTime - startTime);
                    if (sel.selectedKeys().contains(key)) {
                        chan.read(inputData);
                    }
                    sel.selectedKeys().clear();
                }
                return inputData.array();
            } catch (Exception e)
            {
                throw new Exception(TAG + " Couldnt receive data from modem: " + e.getMessage());
            }
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
            s.setSoTimeout(3000);
            int nread = input.read(bytes, dend, h - dend); //blocks

            Log.d("usgov","nread "+String.valueOf(nread));


            //ssize_t nread = server::os->recv(sock, &(*this)[dend], h-dend, 0);
            if (nread <= 0) {
                error = nread == 0 ? 1 : 2;
                Log.i("usgov","FALSE");
                return false;
            }
            dend += nread;
            if (dend < h) return true;
            int sz = decode_size();
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
        int nread = input.read(bytes, dend, bytes.length - dend);
        Log.i("usgov","nread "+String.valueOf(nread));
        if (nread <= 0) {
            error = nread == 0 ? 1 : 2;
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