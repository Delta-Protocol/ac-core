package us.wallet;
import java.io.OutputStream;
import us.gov.cash.tx.sigcode_t;
import java.io.IOException;
import us.gov.socket.datagram;
import java.nio.charset.Charset;

public class wallet_rpc_api implements wallet_api {
        wallet_rpc_api(rpc_api endpoint_) {
            endpoint=endpoint_;
        }

    String to_string(boolean b) {
        return b?"1":"0";
    }

    String to_string(long b) {
        return Long.toString(b);
    }

    String to_string(tx_make_p2pkh_input i) {
        return i.to_string();
    }

    String to_string(sigcode_t i) {
        return new String()+i.asShort();
    }



//---------------------------------------------------------------------generated by make, do not edit
//content of file: ../../api/apitool_generated__functions_wallet_java_rpc-impl
//------------------generated by apitool- do not edit
@Override public void balance(boolean a0, OutputStream a1) {
    String o=new String();
    o += to_string(a0) + ' ';
    endpoint.ask(protocol.wallet_balance, o, a1);
}

@Override public void list(boolean a0, OutputStream a1) {
    String o=new String();
    o += to_string(a0) + ' ';
    endpoint.ask(protocol.wallet_list, o, a1);
}

@Override public void new_address(OutputStream a0) {
    endpoint.ask(protocol.wallet_new_address, a0);
}

@Override public void add_address(String a0, OutputStream a1) {
    endpoint.ask(protocol.wallet_add_address, a0, a1);
}

@Override public void transfer(String a0, long a1, OutputStream a2) {
    String o=new String();
    o += a0 + ' ';
    o += to_string(a1) + ' ';
    endpoint.ask(protocol.wallet_transfer, o, a2);
}

@Override public void tx_make_p2pkh(tx_make_p2pkh_input a0, OutputStream a1) {
    String o=new String();
    o += to_string(a0) + ' ';
    endpoint.ask(protocol.wallet_tx_make_p2pkh, o, a1);
}

@Override public void tx_sign(String a0, sigcode_t a1, sigcode_t a2, OutputStream a3) {
    String o=new String();
    o += a0 + ' ';
    o += to_string(a1) + ' ';
    o += to_string(a2) + ' ';
    endpoint.ask(protocol.wallet_tx_sign, o, a3);
}

@Override public void tx_send(String a0, OutputStream a1) {
    endpoint.ask(protocol.wallet_tx_send, a0, a1);
}

@Override public void tx_decode(String a0, OutputStream a1) {
    endpoint.ask(protocol.wallet_tx_decode, a0, a1);
}

@Override public void tx_check(String a0, OutputStream a1) {
    endpoint.ask(protocol.wallet_tx_check, a0, a1);
}

@Override public void ping(OutputStream a0) {
    ask_ping(a0);
}

//-/----------------generated by apitool- do not edit

//-/-------------------------------------------------------------------generated by make, do not edit

void ask_ping(OutputStream os) {
    try {
    if (!endpoint.connect_walletd(os)) return;
    datagram d=new datagram(protocol.wallet_ping,"");
    rpc_api.pair<String,datagram> r=endpoint.send_recv(d);
    if (r.first!=null) {
        os.write((new String("Error. ") + r.first).getBytes(Charset.forName("UTF-8")));
    }
    else {
       os.write((new String("Remote wallet says: ") + r.second.parse_string()).getBytes(Charset.forName("UTF-8")));
    }
    }
    catch(IOException e) {
    }
}


     rpc_api endpoint;
}