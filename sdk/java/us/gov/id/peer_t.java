package us.gov.id;

//import us.gov.socket.peer_t;
import java.security.GeneralSecurityException;
import java.security.spec.InvalidKeySpecException;
import us.gov.socket.datagram;
import us.gov.crypto.ec;
import us.gov.crypto.base58;
import java.security.KeyPair;
import java.security.PublicKey;
import java.security.PrivateKey;
import java.net.Socket;
import us.gov.protocol;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;

public class peer_t extends us.gov.socket.peer_t {

		public enum stage_t { //coupled with sdk/java/src/Wallet.java
			anonymous,
			verified,
			verified_fail,
			num_stages;

            public short asShort() {
                return (short)ordinal();
            }
            public static stage_t fromShort(short i) {
                return stage_t.values()[i];
            }
		};
        public String[] stagestr = {"anonymous","verified","verified_fail"};

		public peer_t(Socket sock) {
            super(sock);
        }

        @Override
	public String connect(String host, int port, boolean block) {
		String r=super.connect(host,port,block);
		if(r==null) {
			initiate_dialogue();
		}
		return r;
	}


        @Override
        public boolean process_work(datagram d) {
            switch(d.service) {
                case protocol.gov_id_request: process_request(d,get_keys()); break;
                case protocol.gov_id_peer_challenge: process_peer_challenge(d,get_keys()); break;
                case protocol.gov_id_challenge_response: process_challenge_response(d); break;
            default: return false;
            }
            return true;
        }

    public void process_request(datagram d, KeyPair mykeys) {
    	String hello_msg=d.parse_string();
    //System.out.println("ID signing with priv " +ec.instance.to_b58(mykeys.getPrivate())+ " - " +ec.instance.to_b58(mykeys.getPublic()));

    try {
	    String signature =ec.instance.sign_encode(mykeys.getPrivate(),hello_msg);
    //cout << "ID: process_request, msg2sign=" << hello_msg << " created signature=" << signature << endl;
	    msg=get_random_message();
            String os=new String();
            os=ec.instance.to_b58(mykeys.getPublic()) + ' ' + signature + ' ' + msg;
    //System.out.println("ID: process_request. sending id_peer_challenge: " +  os);
    	    send(new datagram(protocol.gov_id_peer_challenge,os));
    }
    catch(GeneralSecurityException e) {
    }
    }

public void process_peer_challenge(datagram d, KeyPair mykeys) {
	String data=d.parse_string();
//System.out.println("ID: process_peer_challenge. received data: " + data);

//	String hello_msg;
//	pubkey_t peer_pubk;
//	String peer_signature_der_b58;

    String[] parts = data.split(" ");
    if (parts.length!=3) {
		stage_peer=stage_t.verified_fail;
        //System.out.println("verification faile:" +parts.length);
        return;
    }
    
    PublicKey peer_pubk;
    try {
        peer_pubk = ec.instance.getPublicKey(base58.decode(parts[0]));
        try {
//System.out.println("Verifying:");
//System.out.println("pubk:"+parts[0]);
//System.out.println("msg:"+msg);
//System.out.println("signature:"+parts[1]);
            //if (ec.instance.verify(peer_pubk,base58.decode(msg),base58.decode(parts[1]))) {
            if (ec.instance.verify(peer_pubk,msg,parts[1])) {
                stage_peer=stage_t.verified;
                pubkey=peer_pubk;
//System.out.println("ID: peer_pubk ok");
            }
            else {
                stage_peer=stage_t.verified_fail;
//System.out.println("ID: peer_pubk and signature verif fail");
        //TODO sleep random time before answering peer , timing attack
            }
        }
        catch(Exception e) {
           stage_peer=stage_t.verified_fail;
        }
    }
    catch(InvalidKeySpecException e) {
       stage_peer=stage_t.verified_fail;
    }
    
   msg=null;

//System.out.println( "ID signing with priv " + ec.instance.to_b58(mykeys.getPrivate()) + " - " +ec.instance.to_b58(mykeys.getPublic()));
    try {
	String signature_der_b58=ec.instance.sign_encode(mykeys.getPrivate(),parts[2]);
	String os=new String();
	os = ec.instance.to_b58(mykeys.getPublic()) + ' ' + signature_der_b58;// + ' ' + stage_peer;
//System.out.println("ID sending id_challenge_response " + os);
	send(new datagram(protocol.gov_id_challenge_response,os));
    }
    catch(GeneralSecurityException e) {
    }

   verification_completed();
}

public void process_challenge_response(datagram d) {
	String data=d.parse_string();
//System.out.println("ID: process_challenge_response " + data);
    String[] parts = data.split(" ");
    if (parts.length!=2) {
        stage_peer=stage_t.verified_fail;
        return;
    }
    
    PublicKey peer_pubk;
    try {
        peer_pubk = ec.instance.getPublicKey(base58.decode(parts[0]));
        try {
            if (ec.instance.verify(peer_pubk,msg,parts[1])) {
                stage_peer=stage_t.verified;
                pubkey=peer_pubk;
//System.out.println("ID: peer_pubk ok");
            }
            else {
                stage_peer=stage_t.verified_fail;
//System.out.println("ID: peer_pubk and signature verif fail");
        //TODO sleep random time before answering peer , timing attack
            }
        }
        catch(Exception e) {
           stage_peer=stage_t.verified_fail;
        }
    }
    catch(InvalidKeySpecException e) {
       stage_peer=stage_t.verified_fail;
    }

    msg=null;
	verification_completed();
}

//public void process_peer_status(datagram d) {
//    stage_me=stage_t.fromShort(d.parse_uint16());

//cout << "ID: process_peer_status " << stage_me << endl;
//cout << "ID: calling verification_completed" << endl;
//	verification_completed();
//}

public void initiate_dialogue() {
//cout << "ID: initiate_dialogue" << endl;
	if (msg==null) {  //if msg not empty we are carrying on the wauth process already
		msg=get_random_message();
		send(new datagram(protocol.gov_id_request,msg));
        try {
        InputStream k=sock.getInputStream();
        }
        catch(Exception e) {
        }
    }
}

public static String get_random_message() {
//return "DUMMY-MSG";  //uncomment for deterministic debugging
    try {
    File file = new File("/dev/urandom");
    FileInputStream urandom = new FileInputStream(file);
    byte[] v = new byte[20];
    if (urandom.read(v) != 20) {
        throw new RuntimeException("/dev/urandom not readable?");
    }
	return base58.encode(v);
    }
    catch(Exception e) {
        return null;
    }
}

        public boolean verification_is_fine() { return stage_peer==stage_t.verified; }

	public void verification_completed() {}

        public KeyPair get_keys() {
            System.exit(1);
            return null; //javac complains if we miss this unreachable code :P
        }

        public String run_auth_responder() {
            while(true) {
                if (stage_peer==stage_t.verified || stage_peer==stage_t.verified_fail) {
                    break;
                }
                pair<String,datagram> r=recv();
                if (r.first!=null) {
                    return r.first;
                }

                if (process_work(r.second)) {
                        continue;
                }
                return "Error. Unrecognized datagram arrived";
            }
            return stage_peer==stage_t.verified?null:"Error. Peer failed to demonstrate identity";
        }

		/// For every edge in the graph of nodes there are two nodes on each end, each node has an instance of this class, 
		/// If one of these nodes is me and the other is peer
		/// stage_me is my own stage (e.g. I am anonymous for him), stage_peer is the other end stage (e.g He is anonymous to me).
		public stage_t stage_peer=stage_t.anonymous;
		//public stage_t stage_me=stage_t.anonymous;

		String msg; //message sent to peer, to be signed by him to check his pubkey.
		public PublicKey pubkey;
}


