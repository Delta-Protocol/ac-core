#include <iostream>

#include "socket.h"			//  	 <us/test/gov/socket/socket.h>

#include <us/gov/socket/datagram.h>

using namespace std;

int main() {

					//  g++ -std=c++17 -lpthread -I../../../.. -L../../../gov -lusgov main.cpp socket.cpp
					// LD_LIBRARY_PATH=../gov ./a.out
	testing_socket_datagram(); 		
	testing_socket_communication();

	exit(0);
}
