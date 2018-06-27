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

#include <us/gov/crypto/base58.h>

Json::Value c::convert_response_move(const string& s) {
    string b58;
    {
    istringstream is(s);
    is >> b58;
    }
    string tx=gov::crypto::b58::decode(b58);
    istringstream is(tx);
    string item;
    is >> item;
    int action;
    is >> action;
    string compartiment;
    is >> compartiment;
    string locking_prog;
    is >> locking_prog;
    string locking_prog_input;
    is >> locking_prog_input;
    string parent_block;    
    is >> parent_block;
    
    Json::Value val;
    val["compartiment"]=compartiment;
    val["item"]=item;
    val["action"]=action==1?"load":"unload";
    val["locking_program"]=locking_prog;
    val["locking_program_input"]=locking_prog_input;
    val["parent_block"]=parent_block;
    return val;
}

