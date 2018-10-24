#ifndef US_GOV_INPUT_CFG_H
#define US_GOV_INPUT_CFG_H

#include <string>
#include <vector>

#include "cfg1.h"

namespace us { namespace gov { namespace input {
using namespace std;

class cfg: public cfg1 {
public:
    cfg(const keys_t::priv_t& privk, const string& home, vector<string>&& seed_nodes);
    cfg(const cfg& other);
    virtual ~cfg();

    static cfg load(const string& home);
    
    const vector<string>& get_seed_nodes() const {
        return m_seed_nodes; 
    } 
private:
    vector<string> m_seed_nodes;
};

typedef cfg cfg_daemon;

}}}

#endif
