#include <iostream>
#include <string>
#include "socket.h"//testing socket h
#include <us/gov/socket/datagram.h>

using namespace std;



	
 void call_datagram(){

	//us::gov::socket::datagram d(1);
	//us::gov::socket::datagram d(10,12);

	us::gov::socket::datagram d(10,"kyprosssf"); //kyprosssf
	

	
	
	
	
	cout << "parse_string()-> " << d.parse_string() << endl;
	
	cout << "parse_uint16() -> " << d.parse_uint16() << endl;


	cout << "completed() -> " << d.completed() << endl;

	cout <<	"  " <<d.send(2) << endl;

}


/*
	test(d.service==10);
	test(d.error==0);
	test(d.size()==8);
	test(d.completed());
	parse_uint16()==12
	parse_string()
*/
	








