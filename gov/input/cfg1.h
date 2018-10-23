#ifndef USGOV_f6953615b74a0604cb0ab84de1c7b7bed3fa40f4ef7e8920ab18e18fa71df3b7
#define USGOV_f6953615b74a0604cb0ab84de1c7b7bed3fa40f4ef7e8920ab18e18fa71df3b7

#include <string>
#include <utility>
#include "cfg0.h"
#include <us/gov/crypto.h>

namespace us { namespace gov { 
namespace input {
using namespace std;

struct cfg1: cfg0 {
    typedef cfg0 b;
	typedef crypto::ec::keys keys_t;
	virtual ~cfg1();
       cfg1(const keys_t::priv_t& privk, const string& home);
        cfg1(const cfg1& other);

        static pair<bool,keys_t::priv_t> load_priv_key(const string& home);

        static string k_file(const string& home);
        static void write_k(const string& home, const keys_t::priv_t& priv);
 
        static cfg1 load(const string& home);
        keys_t keys;

};

typedef cfg1 cfg_id;

}
}}
                                                                

#endif

