#include <iostream>
#include "split_function.h" //dfs
#include "socket.h"
#include <us/gov/socket/datagram.h>

#include "crypto_test.h"
#include <us/gov/crypto/SymmetricEncryption.h>

using namespace std;


int main() {


	testing_split_function(); //dfs
	TestSymmetricEncryption();


	
	//instance of datagram && run it
	// foo f;
	// int y = f.get();	
	

	exit(0);
}


