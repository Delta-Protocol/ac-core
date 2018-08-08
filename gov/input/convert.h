
#ifndef USGOV_47334c096bbab50a71f79cb22e1790fb59c211aff5c75577eb0464efe6bff764
#define USGOV_47334c096bbab50a71f79cb22e1790fb59c211aff5c75577eb0464efe6bff764

#include <us/gov/crypto.h>

namespace us{ namespace gov {
namespace input {
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


}
}}

#endif



