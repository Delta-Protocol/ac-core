package us.gov.auth;
import java.net.Socket;
import java.security.PublicKey;

public class peer_t extends us.gov.id.peer_t {
        public enum stage_t {
            denied,
            authorized,
            num_stages;

            public short asShort() {
                return (short)ordinal();
            }
            public static stage_t fromShort(short i) {
                return stage_t.values()[i];
            }
        };

        public String[] stagestr = {"denied","authorized"};

        public peer_t(Socket sock) {
            super(sock);
        }

        @Override
       public void verification_completed() {
            //super.verification_completed();
            if (!verification_is_fine()) {
                return;
            }
           	if (!authorize(pubkey)) {
                return;
           	}
           	stage=stage_t.authorized;
        }

    	public boolean authorize(PublicKey pub) {
            return false;
        }
        //virtual string run_auth() override;

        public stage_t stage=stage_t.denied;
}
