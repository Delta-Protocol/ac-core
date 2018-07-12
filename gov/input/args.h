#ifndef USGOV_50d37207c1cd1fede08cc12a819a7a44a67234e5ad287e693cd167c2ee630227
#define USGOV_50d37207c1cd1fede08cc12a819a7a44a67234e5ad287e693cd167c2ee630227

#include "args0.h"
#include <us/gov/cash.h>
#include <us/gov/crypto.h>

namespace us { namespace gov {
namespace input {

    using namespace std;
    template<> us::gov::cash::tx::sigcode_t convert(const string& s);
    template<> us::gov::crypto::ec::keys::priv_t convert(const string& s);


}
}}

#endif
