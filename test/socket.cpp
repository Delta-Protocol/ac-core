#include <iostream>
#include <string>
#include "socket.h"//testing socket h
#include <us/gov/socket/datagram.h>

using namespace std;






	
 void call_datagram(){

	
	us::gov::socket::datagram dg(10,12);
	
	cout << dg.decode_size() << endl;


	cout << "completed: " << dg.completed() << endl;
	
}











