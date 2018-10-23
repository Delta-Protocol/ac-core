#ifndef US_GOV_INPUT_CONVERT_H
#define US_GOV_INPUT_CONVERT_H

#include <us/gov/crypto.h>

namespace us{ namespace gov { namespace input {
using namespace std;

template<typename T>
T convert(const string& s) {
    T v;
    istringstream is(s);
    is >> v;
    return move(v);
}

template<> string convert(const string& s);
template<> us::gov::crypto::ec::keys::priv_t convert(const string& s);
template<> us::gov::crypto::ec::keys::pub_t convert(const string& s);

}}}

#endif



