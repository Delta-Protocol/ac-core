
#ifndef USGOV_8e4658449e0f86dedb53914984a000314dc1ebf49683eb585130676e4471924d
#define USGOV_8e4658449e0f86dedb53914984a000314dc1ebf49683eb585130676e4471924d

#include <string>
#include <jsoncpp/json/json.h> 

namespace us { namespace wallet { namespace w3api {

using namespace std;

struct json {

    static Json::Value convert_response_new_compartiment(const string& s);

};

}}}

#endif



