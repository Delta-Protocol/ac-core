#include "args.h"

using namespace us::gov::input;
using namespace std;
using us::gov::cash::tx;
using us::gov::crypto::ec;

template<> tx::sigcode_t us::gov::input::convert(const string& s) {
	if (s==tx::sigcodestr[tx::sigcode_all]) return tx::sigcode_all;
	if (s==tx::sigcodestr[tx::sigcode_none]) return tx::sigcode_none;
	if (s==tx::sigcodestr[tx::sigcode_this]) return tx::sigcode_this;
	return tx::sigcode_all;
}

template<> ec::keys::priv_t us::gov::input::convert(const string& s) {
	return ec::keys::priv_t::from_b58(s);
}


