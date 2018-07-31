package us.gov.socket;

import java.net.Socket;
import java.io.IOException;

	public class client {


		public client() {
            sock=null;
        }

		public client(Socket s) {
            sock=s;
            if (connected()) addr=address();
        }
/*
		virtual ~client() {
			disconnect();
			  //you must override destructor call the virtual function disconnect on the most specialized class
		}
*/
		String connect(String host, int port) {
        	//lock_guard<mutex> lock(mx);
            try {
                sock = new Socket(host, port);
    /*
            	String r=init_sock(host, port, block);
            	if (r!=null) {
                    return r;
                }
*/
            }
            catch (IOException e) {
                return new String("Error. "+e.getMessage());
            }
        	addr=host;
            on_connect();
        	return null;
        }

		void disconnect() {
        //	lock_guard<mutex> lock(mx);
        	if (!connected()) return;
        	sock.close();
        }

        boolean connected() { return sock.isConnected(); }

		String address() {
//            return sock.getHostAddress().getHostAddress();
            return new String("");
        }

		void ready() {}

public class pair<f,s> {
    public f first;
    public s second;

    public pair(f fst, s snd) {
        this.first=fst;
        this.second=snd;
     }
 }


pair<String,datagram> recv(short expected_service) {
    pair<String,datagram> r=recv();
    if (r.first!=null) {
        return r;
    }
    datagram d=r.second;
    while(true) { ///delete garbage injected by hackers, discovered when gov trolled wallet with vote mesages
        if (d.service==protocol.protocol_gov_socket_error) {
            return r;
        }
        if (d.service==expected_service) {
            break;
        }
        pair<String,datagram> r2=recv();
        if (r2.first!=null) {
            return r2;
        }
        d=r2.second;
    }
    return new pair<String,datagram>(null,d);
}



pair<String,datagram> send_recv(datagram d,short expected_service) {
    pair<String,datagram> ans;
    ans.first=send(d);
    if (ans.first!=null) {
        ans.second=null;
        return ans;
    }
    return recv(expected_service);
}

pair<String,datagram> send_recv(datagram d) {
    pair<String,datagram> ans;
    ans.first=send(d);
    if (ans.first!=null) {
        ans.second=null;
        return ans;
    }
    return recv();
}

String send(datagram d) {
	if (!connected()) {
		return new String("Error. Sending datagram before connecting.");
	}
	pair<String,datagram> r=d.sendto(sock);
    if (r!=null) {
        disconnect();
    }
	return r;
}

pair<String,datagram> recv() { //caller owns the returning object
    pair<String,datagram> r;
    r.second=new datagram();
    while(true) {
        String ans=r.second.recvfrom(sock); //,socket::response_timeout_secs);
        if (ans==null) {
            r.first=ans;
            r.second=null;
            disconnect();
            break;
        }
        if (r.second.completed()) {
            break;
        }
    }
    return move(r);
}

//		void init_sockaddr (struct sockaddr_in *name, const char *hostname, uint16_t port);
//		string init_sock(const string& host, uint16_t port, bool block=false);

		void on_connect() {}
/*
		void dump(ostream& os) const;
                virtual void dump_all(ostream& os) const {
                        dump(os);
                }
*/
        Socket sock;
		String msg;
		String addr;
		//mutable mutex mx;
	}





