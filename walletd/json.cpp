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
    val["transaction_b58"]=s;
    val["transaction"]=tx;
    val["compartiment"]=compartiment;
    val["item"]=item;
    val["action"]=action==1?"load":"unload";
    val["locking_program"]=locking_prog;
    val["locking_program_input"]=locking_prog_input;
    val["parent_block"]=parent_block;
    return val;
}

Json::Value c::convert_response_track(const string& s) {
    string b58;
    {
    istringstream is(s);
    is >> b58;
    }
    string tx=gov::crypto::b58::decode(b58);
    istringstream is(tx);
    string payloadb58;
    is >> payloadb58;
    string compartiment;
    is >> compartiment;
    string locking_prog;
    is >> locking_prog;
    string locking_prog_input;
    is >> locking_prog_input;
    string parent_block;    
    is >> parent_block;
    
    Json::Value val;
    val["transaction_b58"]=b58;
    val["compartiment"]=compartiment;
    val["data_b58"]=payloadb58;
    val["locking_program"]=locking_prog;
    val["locking_program_input"]=locking_prog_input;
    val["parent_block"]=parent_block;
    return val;
}

#include <us/gov/nova/app.h>

Json::Value c::convert_response_query(const string& s) {
    Json::Value val;
	istringstream is(s);
	int m=0;
	is >> m;
	Json::Value cos;
	
	for (int i=0; i<m; ++i) {
    	Json::Value co;
		string comp;
		is >> comp;
		gov::nova::app::compartiment_t a=gov::nova::app::compartiment_t::from_stream(is);
		co["id"]=comp;

		    Json::Value lb;
		    int n=0;
		    for (auto&i:a.logbook) {
		        lb[n++]=i;
		    }
		    co["logbook"]=lb;

	    Json::Value itms;
	    n=0;
	    for (auto&i:a.logbook.items) {
	        itms[n++]=i;
	    }
	    co["items"]=itms;
        cos[i]=co;

	}

	val["compartiments"]=cos;
//    val["locking_program"]=a.locking_program;

//    string parent_block;    
//    is >> parent_block;
    return val;
}


Json::Value c::convert_response_mempool(const string& s) {
    Json::Value val;
    val["raw"]=s;
//return val;
	istringstream is(s);
	int m=0;
	is >> m;
	Json::Value cos;
	
	for (int i=0; i<m; ++i) {
    	Json::Value co;
		string comp;
		is >> comp;
		gov::nova::app::compartiment_t a=gov::nova::app::compartiment_t::from_stream(is);
		co["id"]=comp;

		    Json::Value lb;
		    int n=0;
		    for (auto&i:a.logbook) {
		        lb[n++]=i;
		    }
		    co["logbook"]=lb;

	    Json::Value itms;
	    n=0;
	    for (auto&i:a.logbook.items) {
	        itms[n++]=i;
	    }
	    co["items"]=itms;
        cos[i]=co;

	}

	val["compartiments"]=cos;
//    val["locking_program"]=a.locking_program;

//    string parent_block;    
//    is >> parent_block;
    return val;
}

