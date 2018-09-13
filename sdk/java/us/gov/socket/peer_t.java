package us.gov.socket;

import java.net.Socket;
import us.gov.protocol;

public class peer_t extends client {

public peer_t() {
}

public peer_t(Socket sock) {
	super(sock);
}

public boolean is_slow() {
    return false;
}

public boolean ping() {
    return send(new datagram(protocol.gov_socket_ping,"ping"))==null;
}

public void process_pong() {
}

public boolean process_work(datagram d) {
     switch(d.service) {
         case protocol.gov_socket_ping: {
            send(new datagram(protocol.gov_socket_pong,"pong"));
            break;
         }
         case protocol.gov_socket_pong: {
            process_pong();
            break;
         }
         default: return false;
     }
     return true;
}

}

