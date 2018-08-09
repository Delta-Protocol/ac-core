#include "convert.h"
#include <us/gov/cash/tx.h>

using namespace us::gov::input;
using namespace std;
using us::gov::cash::tx;

template<> tx::sigcode_t us::gov::input::convert(const string& s) {
    if (s==tx::sigcodestr[tx::sigcode_all]) return tx::sigcode_all;
    if (s==tx::sigcodestr[tx::sigcode_none]) return tx::sigcode_none;
    if (s==tx::sigcodestr[tx::sigcode_this]) return tx::sigcode_this;
    return tx::sigcode_all;
}


