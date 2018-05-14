#include <iostream>
#include <string>
#include "socket.h"//testing socket h
#include <us/gov/socket/datagram.h>

using namespace std;






void test_1(int serviceNumber){
	us::gov::socket::datagram d(serviceNumber);

	cout << "parse_string()-> " << d.parse_string() << endl;
	cout << "parse_uint16() -> " << d.parse_uint16() << endl;
	cout <<	" jjj " <<d.send(2) << endl;
	cout << "service -> " << d.service << endl;
	cout << "completed() -> " << d.completed() << endl;

}



void test_2(int serviceNumber, int payloadNumber){
	us::gov::socket::datagram d(serviceNumber, payloadNumber);

	cout << "parse_string()-> " << d.parse_string() << endl;
	cout << "parse_uint16() -> " << d.parse_uint16() << endl;
	cout <<	" jjj " <<d.send(2) << endl;
	


	cout << "service -> " << d.service << endl;
	cout << "dend -> " << d.dend << endl;

	cout << "completed() -> " << d.completed() << endl;
}




void test_3(int serviceNumber , string payloadString){
	us::gov::socket::datagram d(serviceNumber, payloadString);

	cout << "parse_string()-> " << d.parse_string() << endl;
	cout << "maxsize  " << d.maxsize << endl;
	
	cout << "h  + payload -> size    " << d.h + d.size() << endl;

	cout << "parse_uint16() -> " << d.parse_uint16() << endl;
	cout <<	" jjj " <<d.send(2) << endl;
	cout << "service -> " << d.service << endl;


	cout << "error : " << d.error << endl;

	cout << "resize : " << d.h + payloadString.size() << endl;

	cout << "dend -> " << d.dend << endl;

	cout << "completed() -> " << d.completed() << endl;

}





	
 void call_datagram(){


	//......1.....datagram(uint16_t service);
	//us::gov::socket::datagram d(10);
//	test_1(10);







	//......2.....datagram(uint16_t service, uint16_t);  ,payload
	//us::gov::socket::datagram d(10,"10");
//	test_2(10,5);







	//......3.....datagram(uint16_t service, const string&); ,payload string
	//us::gov::socket::datagram d(10,"kyprosssf");
	test_3(10,"testingPayload");	


	
}


/*
	test(d.service==10);
	test(d.error==0);
	test(d.size()==8);
	test(d.completed());
	parse_uint16()==12
	parse_string()
*/




