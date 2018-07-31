package us.wallet;

    public class tx_make_p2pkh_input {
        public static final int sigcode_all=0;
        public static final int sigcode_none=1;
        public static final int sigcode_this=2;

        public tx_make_p2pkh_input(String rcpt_addr0, int amount0, int fee0, int sigcode_inputs0, int sigcode_outputs0, boolean sendover0) {
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
        int amount;
        int fee;
        int sigcode_inputs;
        int sigcode_outputs;
        boolean sendover;

        boolean check() {
            return fee>0 && amount>0 && !rcpt_addr.isEmpty();
        }

        String to_string() {
            return rcpt_addr+" "+amount+" "+fee+" "+sigcode_all+" "+sigcode_all+" "+(sendover?"1":"0");
        }
    }
