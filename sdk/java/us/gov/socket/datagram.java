package us.gov.socket;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.PrintWriter;
import java.net.Socket;
import java.io.InputStream;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.Arrays;

public class datagram {

    static final int h=6;
    static final int maxsize=100000;

    byte[] bytes;
    int dend;
    public short service;

    public datagram() {
        bytes=new byte[h];
        dend=0;
    }
    public datagram(short service) {
        bytes=new byte[h];
        encode_size(h);
        encode_service(service);
        dend=bytes.length;
    }
    public datagram(short service, String msg) {
        int size=h+msg.length();
        bytes=new byte[size];
        encode_size(size);
        encode_service(service);
        System.arraycopy(msg.getBytes(), 0, bytes, h, msg.length());
        dend=bytes.length;
    }

    public datagram(short service, short payload) {
        int size=h+2;
        bytes=new byte[size];
        encode_size(size);
        encode_service(service);

        byte[] t=new byte[2];
        ByteBuffer bb = ByteBuffer.wrap(t);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        bb.asShortBuffer().put(payload);
        bytes[h]=t[0];
        bytes[h+1]=t[1];
        dend=bytes.length;
    }

    public void encode_size(int sz) {
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
    }

    public int decode_size() {
        assert bytes.length>3;
        byte[] t=new byte[4];
        t[0]=bytes[0];
        t[1]=bytes[1];
        t[2]=bytes[2];
        t[3]=bytes[3];
        ByteBuffer bb = ByteBuffer.wrap(t);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        return bb.getInt();
    }

    public long size() {
        return bytes.length;
    }


    public void encode_service(short svc) {
        assert h==6;
        assert(bytes.length>=h);

        byte[] t=new byte[2];
        ByteBuffer bb = ByteBuffer.wrap(t);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        bb.asShortBuffer().put(svc);
        bytes[4+0]=t[0];
        bytes[4+1]=t[1];
    }

    short decode_service() {
        assert bytes.length>5;
        byte[] t=new byte[2];
        t[0]=bytes[4+0];
        t[1]=bytes[4+1];
        ByteBuffer bb = ByteBuffer.wrap(t);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        return bb.getShort();
    }

    public boolean completed() {
        return dend==bytes.length && bytes.length>0;
    }

public String sendto(Socket sock) {
    if (bytes.length>=maxsize) {
            return "Error. Datagram is too big.";
    }
   try {
        DataOutputStream dOut = new DataOutputStream(sock.getOutputStream());
        dOut.write(bytes);
    }
    catch(Exception e) {
        return e.getMessage();
    }

    return null;
}
/*
    public void send(Socket s) {
	try {
		DataOutputStream dOut = new DataOutputStream(s.getOutputStream());
		dOut.write(bytes);
        }
        catch(Exception e) {
        }
    }
*/


    String recvfrom(Socket s) {
        try {
        DataInputStream input = new DataInputStream(s.getInputStream());
        if (dend < h) {
            s.setSoTimeout(3000);
            int nread = input.read(bytes, dend, h - dend); //blocks
            if (nread <= 0) {
                return "Timeout waiting for data from peer. or Connection ended by peer.";
            }
            dend += nread;
            if (dend < h) return null;
            int sz = decode_size();
            if (sz > maxsize) {
                return "Error. Incoming datagram exceeded size limit.";
            }
            byte[] m = new byte[sz];
            System.arraycopy(bytes, 0, m, 0, h);
            bytes = m;
            service = decode_service();
            if (dend == sz) {
                return null;
            }
        }

        int nread = input.read(bytes, dend, bytes.length - dend);
        if (nread <= 0) {
            return "Error.3 Timeout waiting for data from peer. or Incoming datagram is too big";
        }
        dend += nread;
        return null;
    }
    catch(Exception e) {
        return "Error. "+e.getMessage();
    }
    }


    public String parse_string() {
        byte[] b=new byte[dend-h];
        System.arraycopy(bytes, h, b, 0, dend-h);

        return new String(b);
    }

    public short parse_uint16() {
        if (bytes.length!=h+2) return 0;

        byte[] t=new byte[2];
        t[0]=bytes[h];
        t[1]=bytes[h+1];
        ByteBuffer bb = ByteBuffer.wrap(t);
        bb.order(ByteOrder.LITTLE_ENDIAN);
        return bb.getShort();
    }

    static char x[]={'P','E','Q','_'};

public String service_str() {
    return service_str(service);
}

public String service_str(short svc) {
    String os=new String();
    if (svc>=100) {
        os = x[svc&3] + ".";
        int m=svc>>2;
        int l=m/100;
        os+= l + "." + m%100;
    }
    else {
        os="wallet";
    }
    return os;
}


};
