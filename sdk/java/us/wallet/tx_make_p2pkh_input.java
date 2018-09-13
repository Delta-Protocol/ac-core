package us.wallet;
import us.gov.cash.tx.sigcode_t;

    public class tx_make_p2pkh_input {

        public tx_make_p2pkh_input(String rcpt_addr0, long amount0, long fee0, sigcode_t sigcode_inputs0, sigcode_t sigcode_outputs0, boolean sendover0) {
            rcpt_addr=rcpt_addr0;
            amount=amount0;
            fee=fee0;
            if (amount<1) amount=0;
            if (fee<1) fee=0;
            sigcode_inputs=sigcode_inputs0;
            sigcode_outputs=sigcode_outputs0;
            sendover=sendover0;
        }

        String rcpt_addr;
        long amount;
        long fee;
        sigcode_t sigcode_inputs;
        sigcode_t sigcode_outputs;
        boolean sendover;

        boolean check() {
            return fee>0 && amount>0 && !rcpt_addr.isEmpty();
        }

        String to_string() {
            return rcpt_addr+" "+amount+" "+fee+" "+sigcode_inputs.asShort()+" "+sigcode_outputs.asShort()+" "+(sendover?"1":"0");
        }
    }
