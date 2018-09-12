package us.wallet;

import us.gov.auth.peer_t;
import us.gov.socket.datagram;
import java.security.KeyPair;
import java.security.PublicKey;
import java.io.OutputStream;
import java.time.Instant;
import java.nio.charset.Charset;
import java.io.IOException;
import java.security.KeyPair;
import java.net.Socket;

public class rpc_api extends us.gov.auth.peer_t {

	public rpc_api(KeyPair k, String walletd_host_, short walletd_port_) {
		super(new Socket());
		walletd_host=walletd_host_;
		walletd_port=walletd_port_;
		id=k;
	}

	@Override
    	public KeyPair get_keys()  {
	   return id;
	}

	@Override
   	public boolean authorize(PublicKey p) {
		return true;
	}

	public void ask(short service, OutputStream os) {
		ask(service,"",os);
	}

	public void ask(short service, String args, OutputStream os) {
        try {
		if(!connect_walletd(os)) return;
		datagram d=new datagram(service,args);
		pair<String, datagram> r=send_recv(d);
		if(r.first!=null) {
			os.write(r.first.getBytes(Charset.forName("UTF-8")));
		}
		else {
			os.write(r.second.parse_string().getBytes(Charset.forName("UTF-8")));
		}
        }
        catch(IOException e) {
        }
	}
/*
    String ask(short service, String args) {
        Datagram d=new Datagram(service,args);
        Datagram r=send_recv(d);
        if (r==null) return "?";
        String st;
        st = r.parse_string();
        //Log.d("Wallet","ans "+st);
        return st.trim();
    }
*/
	public boolean connect_walletd(OutputStream os) throws IOException {
		if(connected()) return true;
		String r=connect(walletd_host,walletd_port,true);
		if(r!=null) {
			os.write((new String("Error. ") + r).getBytes(Charset.forName("UTF-8")));
			return false;
		}
		r=run_auth_responder();
		if(r!=null) {
			os.write(r.getBytes(Charset.forName("UTF-8")));
			return false;
		}
		if(!connected()) {
			os.write(new String("Auth failed").getBytes(Charset.forName("UTF-8")));
			return false;
		}
		connected_since=Instant.now();
		return true;
	}

	String walletd_host;
	short walletd_port;

	KeyPair id;
	Instant connected_since;

};

