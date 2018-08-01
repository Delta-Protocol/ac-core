#include <iostream>
#include "split_function.h" 		//dfs
#include "socket.h"            		//socket
#include <us/gov/socket/datagram.h>
#include <us/gov/crypto/symmetric_encryption.h>
#include <us/gov/crypto/ec.h>

#include "symmetric_encryption_test.h"
#include "elliptic_cryptography_test.h"



using namespace std;


int main() {

	//testing_split_function(); //dfs
	test_symmetric_encryption();
	test_elliptic_cryptography();
	
	exit(0);
}


