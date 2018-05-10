#include "args.h"

using namespace uswallet;
using namespace usgov;
using namespace std;
typedef uswallet::args_t c;


template<> string uswallet::convert(const string& s) {
	return s;
}

template<> cash::tx::sigcode_t uswallet::convert(const string& s) {
	if (s==cash::tx::sigcodestr[cash::tx::sigcode_all]) return cash::tx::sigcode_all;
	if (s==cash::tx::sigcodestr[cash::tx::sigcode_none]) return cash::tx::sigcode_none;
	if (s==cash::tx::sigcodestr[cash::tx::sigcode_this]) return cash::tx::sigcode_this;
	return cash::tx::sigcode_all;
}

template<> crypto::ec::keys::priv_t uswallet::convert(const string& s) {
	return crypto::ec::keys::priv_t::from_b58(s);
}

