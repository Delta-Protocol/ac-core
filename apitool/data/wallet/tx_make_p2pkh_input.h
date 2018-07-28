    struct tx_make_p2pkh_input {
        typedef  us::gov::cash::hash_t hash_t;
        typedef  us::gov::cash::cash_t cash_t;
        typedef  us::gov::cash::tx::sigcode_t sigcode_t;

        hash_t rcpt_addr;
        cash_t amount;
        cash_t fee;
        sigcode_t sigcode_inputs;
        sigcode_t sigcode_outputs;
        bool sendover;

        void to_stream(ostream&) const;
        static tx_make_p2pkh_input from_stream(istream&);
    };

