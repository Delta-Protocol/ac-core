#include <iostream>
#include <string>
#include "socket.h"//testing socket h
#include <us/gov/socket/datagram.h>

using namespace std;





void test_1(int serviceNumber){
	us::gov::socket::datagram d(serviceNumber);
	cout << "\n-------Test-1------ "<< endl;
	
	//constructor results
	cout << "resize(h) ----------->  " << d.h << endl;
	cout << " dend=size() :------->  " << d.dend << "  " << d.size() << endl;

	//rest of the code--------------------------------------
	cout << "parse_uint16() -> " << d.parse_uint16() << endl;
	cout << "maxsize        -> " << d.maxsize << endl;
	cout << "service        -> " << d.service << endl;
	cout << "error          -> " << d.error << endl;
	cout << "compute_hash() -> " << d.compute_hash() << endl;
	cout << "bool send      -> " << d.send(10) << endl;
	cout << "bool recv(int sock)-> " << d.recv(10) << endl;
	cout << "parse_uint16()	-> "  << d.parse_uint16() << endl;
  	//recv(int sock, int timeout_seconds)
	//parse_strings() 
	cout << "completed()    -> " << d.completed() << endl;
}



void test_2(int serviceNumber, int payloadNumber){
	us::gov::socket::datagram d(serviceNumber, payloadNumber);
	cout << "\n-------Test-2------ "<< endl;

	//constructor results
	cout << "resize(h+2) --------------->  " << d.h+2 << endl;
	//cout << "(*this)[h]=payload&0xff --->  " << payloadNumber&0xff << endl;	
	//cout << "(*this)[h+1]=payload>>8 --->  " << payloadNumber>>8 << endl; 
	cout << " dend=size() :------------->  " << d.dend << "  " << d.size() << endl;

	//rest of the code--------------------------------------
	cout << "parse_uint16() -> " << d.parse_uint16() << endl;
	cout << "maxsize        -> " << d.maxsize << endl;
	cout << "service        -> " << d.service << endl;
	cout << "error          -> " << d.error << endl;
	cout << "compute_hash() -> " << d.compute_hash() << endl;
	cout << "bool send      -> " << d.send(10) << endl;
	cout << "bool recv(int sock)-> " << d.recv(10) << endl;
	cout << "parse_uint16()	-> "  << d.parse_uint16() << endl;
  	//recv(int sock, int timeout_seconds)
	//parse_strings() 
	cout << "completed()    -> " << d.completed() << endl;
}




void test_3(int serviceNumber , string payloadString){
	us::gov::socket::datagram d(serviceNumber, payloadString);
	cout << "\n-------Test-3------ "<< endl;

	//constructor results--------------------------------
	cout << "h + payload.size():--------------> " << d.h + payloadString.size() << "   < maxsize: " << d.maxsize  << endl; 
	cout << "resize(h+payload.size()):--------> " << d.h + payloadString.size() << endl;
	cout << "  h :----------------------------> " << d.h << endl;
	cout << "d.payloadString.size() ----------> " << payloadString.size()<< endl;
	
	if( d.size() == d.h) 
	{
		cout << "d.size() == datagram::h" << endl;
	}

	cout << " d.size()    :------------------> " << d.size() << endl;
	cout << " dend=size() :------------------> " << d.dend << "  " << d.size() << endl;


	

	//rest of the code--------------------------------------
	cout << "parse_string() -> " << d.parse_string() << endl;
	cout << "maxsize        -> " << d.maxsize << endl;
	cout << "service        -> " << d.service << endl;
	cout << "error          -> " << d.error << endl;
	cout << "compute_hash()	-> " << d.compute_hash() << endl;
	cout << "bool send      -> " << d.send(10) << endl;
	cout << "bool recv(int sock)-> " << d.recv(10) << endl;
	cout << "parse_uint16()	-> "  << d.parse_uint16() << endl;
  	//recv(int sock, int timeout_seconds)
	//parse_strings() 
	cout << "completed()	-> " << d.completed() << endl;
}



/*
bool test_int(const int& input, const int& expected ) {
	
	if( us::gov::socket::datagram::FUNCTION_NAME(input) != expected )  {  	 
		assert (false);
	 }
   return true;
}
*/



//......................................................:::::Main Function:::::...........................................	
 void testing_socket_datagram(){

	

	//......1.....datagram(uint16_t service);
	test_1(100);//65536 -> 2^16   maximu=4294967296 -> 2^32



	//......2.....datagram(uint16_t service, uint16_t);  ,payload
	test_2(100,5);



	//......3.....datagram(uint16_t service, const string&); ,payload string
	test_3(101,"zzz");	


	
}
