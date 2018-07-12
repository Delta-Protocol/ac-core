#ifndef USGOV_3129cf05f2fadec9ea29b790fb8f5a4e5429305f17c8ba082a9800a1f455dbe1
#define USGOV_3129cf05f2fadec9ea29b790fb8f5a4e5429305f17c8ba082a9800a1f455dbe1

#include <string>
#include <vector>
#include "cfg1.h"

namespace us { namespace gov { 
namespace input {
using namespace std;

struct cfg: cfg1 {
    typedef cfg1 b;

    cfg(const keys_t::priv_t& privk, const string& home, vector<string>&& seed_nodes);
    cfg(const cfg& other);
    virtual ~cfg();

    static cfg load(const string& home);
    vector<string> seed_nodes;
};


}
}}
                                                                

#endif

