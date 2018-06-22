#include <iostream>
#include "split_function.h" //dfs
#include "socket.h"
#include <us/gov/socket/datagram.h>

#include "crypto_test.h"
#include <us/gov/crypto/symmetric_encryption.h>

using namespace std;


int main() {

	testing_split_function(); //dfs
	test_symmetric_encryption();

	//instance of datagram && run it
	// foo f;
	// int y = f.get();	

	exit(0);
}


