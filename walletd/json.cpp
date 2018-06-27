#include "json.h"

using namespace std;
using namespace us::wallet::w3api;
typedef us::wallet::w3api::json c;
 
Json::Value c::convert_response_new_compartiment(const string& s) {
    Json::Value val;
    val["compartiment"]=s;
    return val;
}

