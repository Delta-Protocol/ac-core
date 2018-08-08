#ifndef USGOV_295e7aaeb03ee968854f88efb260a727b72d3462dca2a2504a293306aa97060e
#define USGOV_295e7aaeb03ee968854f88efb260a727b72d3462dca2a2504a293306aa97060e

#include <us/gov/input/convert.h>
#include <us/gov/cash/tx.h>

namespace us{ namespace gov {
namespace input {
using namespace std;

template<> us::gov::cash::tx::sigcode_t convert(const string& s);

}
}}

#endif



