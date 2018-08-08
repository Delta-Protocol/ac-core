#include <iostream>
#include "split_function.h"
#include "socket.h"
#include <us/gov/socket/datagram.h>
#include <us/gov/crypto/symmetric_encryption.h>
#include <us/gov/crypto/ec.h>
#include "symmetric_encryption_test.h"
#include "elliptic_cryptography_test.h"

using namespace std;

int main() {

	test_symmetric_encryption();
	test_elliptic_cryptography();
	testing_split_function();
	testing_socket_datagram(); 		
	testing_socket_communication();

	exit(0);
}
