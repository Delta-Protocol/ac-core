#include "args.h"

using namespace us::wallet;
using namespace us::gov;
using namespace std;
typedef us::wallet::args_t c;


template<> string us::wallet::convert(const string& s) {
	return s;
}

template<> cash::tx::sigcode_t us::wallet::convert(const string& s) {
	if (s==cash::tx::sigcodestr[cash::tx::sigcode_all]) return cash::tx::sigcode_all;
	if (s==cash::tx::sigcodestr[cash::tx::sigcode_none]) return cash::tx::sigcode_none;
	if (s==cash::tx::sigcodestr[cash::tx::sigcode_this]) return cash::tx::sigcode_this;
	return cash::tx::sigcode_all;
}

template<> crypto::ec::keys::priv_t us::wallet::convert(const string& s) {
	return crypto::ec::keys::priv_t::from_b58(s);
}

