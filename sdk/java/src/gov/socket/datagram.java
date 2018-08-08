package us.gov.socket;

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

public class datagram {

    static final int h=6;
    static final int maxsize=100000;

    byte[] bytes;
    int dend;
    int error;
    int service;

    public datagram() {
        bytes=new byte[h];
        dend=0;
        error=0;
    }
    public datagram(short service) {
        int size=6;
        bytes= new byte[size];
        encode_size(size);
        encode_service(service);
        dend=bytes.length;
    }
    public datagram(short service, String msg) {
        int size=6+msg.length();
        bytes= new byte[size];
        encode_size(size);
        encode_service(service);
        System.arraycopy(msg.getBytes(), 0, bytes, 6, msg.length());
        dend=bytes.length;
    }

    public boolean completed() {
        return dend==bytes.length && bytes.length>0;
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

    public void send(Socket s) {
	try {
		DataOutputStream dOut = new DataOutputStream(s.getOutputStream());
		dOut.write(bytes);
        }
        catch(Exception e) {
        }
    }

    boolean recv(Socket s) {
        try {
        DataInputStream input = new DataInputStream(s.getInputStream());
        if (dend < h) {
            s.setSoTimeout(3000);
            int nread = input.read(bytes, dend, h - dend); //blocks
            if (nread <= 0) {
                error = nread == 0 ? 1 : 2;
                return false;
            }
            dend += nread;
            if (dend < h) return true;
            int sz = decode_size();
            if (sz > maxsize) {
                error = 3;
                return false;
            }
            byte[] m = new byte[sz];
            System.arraycopy(bytes, 0, m, 0, h);
            bytes = m;
            service = decode_service();
            if (dend == sz) {
                return true;
            }
        }
        int nread = input.read(bytes, dend, bytes.length - dend);
        if (nread <= 0) {
            error = nread == 0 ? 1 : 2;
            return false;
        }
        dend += nread;
        return true;
    }
    catch(Exception e) {
        return false;
    }
    }

    String parse_string() {
        byte[] b=new byte[dend-h];
        System.arraycopy(bytes, h, b, 0, dend-h);

        return new String(b);
    }
};
