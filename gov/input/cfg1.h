#ifndef US_GOV_INPUT_CFG1_H
#define US_GOV_INPUT_CFG1_H

#include <string>
#include <utility>

#include "us/gov/crypto/ec.h"
#include "cfg0.h"

namespace us { namespace gov { namespace input {
using namespace std;

class cfg1: public cfg0 {
public:

    typedef crypto::ec::keys keys_t;

    cfg1(const keys_t::priv_t& privk, const string& home);
    cfg1(const cfg1& other);
    virtual ~cfg1();

    static pair<bool,keys_t::priv_t> load_priv_key(const string& home);
    static string k_file(const string& home);
    static void write_k(const string& home, const keys_t::priv_t& priv);
 
    static cfg1 load(const string& home);

    const keys_t& get_keys() const {
        return m_keys;
    }
private:
    keys_t m_keys;
};

typedef cfg1 cfg_id;

}}}

#endif

