#ifndef USGOV_35983b282eac9c91c0e90893cd4b3586191237650a0dc3908a66e683a0dd15b5
#define USGOV_35983b282eac9c91c0e90893cd4b3586191237650a0dc3908a66e683a0dd15b5

#include <string>
#include <iostream>

namespace us{ namespace gov {
namespace engine {
using namespace std;

        struct addr_t {
            virtual ~addr_t() {}
            virtual void to_stream(ostream& os) const=0;
            static addr_t* from_stream(istream& is);
        };


    ostream& operator << (ostream& os, const addr_t& o);

}
}}



#endif



