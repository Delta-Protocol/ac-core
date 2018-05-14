#include <iostream>
#include <string>
#include <us/gov/dfs/daemon.h>

#include "crypto_test.h"

using namespace std;



bool test(const string& input, const string& expected ) {
	
	if( us::gov::crypto::daemon::resolve_filename(input) != expected )  {  	 
		assert (false);
	 }
   return true;
}

void testing_symmetric_encryption(){

}