#ifndef US_GOV_ENGINE_ADDR_T_H
#define US_GOV_ENGINE_ADDR_T_H

#include <string>
#include <iostream>

namespace us{ namespace gov{ namespace engine{
using namespace std;

class addr_t {
public:
    virtual ~addr_t() {}
    virtual void to_stream(ostream& os) const=0;
    static addr_t* from_stream(istream& is);
};

ostream& operator << (ostream& os, const addr_t& o);

}}}

#endif



