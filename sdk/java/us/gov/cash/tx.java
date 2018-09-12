package us.gov.cash;

public class tx {
        public enum sigcode_t {
            sigcode_all,
            sigcode_none,
            sigcode_this,
            num_sigcodes;

            public short asShort() {
                return (short)ordinal();
            }
            public static sigcode_t fromShort(short i) {
                return sigcode_t.values()[i];
            }
        };
        public String[] sigcodestr = {"all","none","this"};

}
