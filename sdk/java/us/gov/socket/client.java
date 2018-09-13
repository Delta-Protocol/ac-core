package us.gov.socket;

import java.net.Socket;
import java.io.IOException;
import us.gov.protocol;

public class client {
        public client() {
//            sock=0;
        }

	public client(Socket sock_) {
		sock=sock_;
		if (connected()) addr=address();
	}

	public String connect(String host, int port) {
		//addr=host;
		return connect(host,port,false);
	}

	public String connect(String host, int port, boolean block) {
        if (connected()) return "Already connected";
		addr=host;
            try {
                sock = new Socket(host, port);
            }
            catch (IOException e) {
                return e.getMessage();
            }
        	addr=host;
       		//on_connect();
        	return null;
        }

	public void disconnect() {
        //	lock_guard<mutex> lock(mx);
        	if (!connected()) return;
            try {
        	    sock.close();
            }
            catch (IOException e) {
            }
        }

        public boolean connected() { return sock.isConnected(); }

	public String address() {
            return sock.getInetAddress().getHostAddress();
        }

	public void ready() {}

public class pair<f,s> {
    public f first;
    public s second;

    public pair(f fst, s snd) {
        this.first=fst;
        this.second=snd;
     }
 }


public pair<String,datagram> recv(short expected_service) {
    pair<String,datagram> r=recv();
    if (r.first!=null) {
        return r;
    }
    datagram d=r.second;
    while(true) { ///delete garbage injected by hackers, discovered when gov trolled wallet with vote mesages
        if (d.service==protocol.gov_socket_error) {
            return r;
        }
        if (d.service==expected_service) {
            break;
        }
        //else {
            //delete d;
        //}
        pair<String,datagram> r2=recv();
        if (r2.first!=null) {
            return r2;
        }
        d=r2.second;
    }
    return new pair<String,datagram>(null,d);
}



public pair<String,datagram> send_recv(datagram d,short expected_service) {
    String r=send(d);
    if (r!=null) {
        return new pair<String,datagram>(r,null);
    }
    return recv(expected_service);
}

public pair<String,datagram> send_recv(datagram d) {
    String r=send(d);
    if (r!=null) {
        return new pair<String,datagram>(r,null);
    }
    return recv();
}

void dump_d(String prefix, datagram d, String addr) {
/*
    System.out.println("SOCKET: " + prefix + " datagram " + d.service + " " + d.service_str() + " of size " + d.size() + " bytes. to " + addr);
    System.out.println("      : " + d.parse_string());
*/
}

public String send(datagram d) {
	if (!connected()) {
		return new String("Error. Sending datagram before connecting.");
	}
	String r=d.sendto(sock);
    if (r!=null) {
        disconnect();
    }
    else {
        if (DEBUG) dump_d("sent",d,addr);
    }
	return r;
}

public pair<String,datagram> recv() { //caller owns the returning object
    pair<String,datagram> r=new pair<String,datagram>(null,new datagram());
    while(true) {
        String ans=r.second.recvfrom(sock); //,socket::response_timeout_secs);
        if (ans!=null) {
            r.first=ans;
            r.second=null;
            disconnect();
            break;
        }
        if (r.second.completed()) {
            if (DEBUG) dump_d("recv",r.second,addr);
            break;
        }
    }
    return r;
}

//		void init_sockaddr (struct sockaddr_in *name, const char *hostname, uint16_t port);
//		string init_sock(const string& host, uint16_t port, bool block=false);

        //public void on_connect() {}
    public Socket sock;
	String msg;
	String addr;
    public static final boolean DEBUG = true;
}

