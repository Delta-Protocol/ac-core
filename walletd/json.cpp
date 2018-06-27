#include "json.h"
#include <sstream>

using namespace std;
using namespace us::wallet::w3api;
typedef us::wallet::w3api::json c;
 
Json::Value c::convert_response_new_compartiment(const string& s) {
    istringstream is(s);
    string p;
    is >> p;
    Json::Value val;
    val["compartiment"]=p;
    return val;
}

