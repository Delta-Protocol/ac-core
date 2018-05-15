#include <iostream>
#include <string>
#include "socket.h"//testing socket h
#include <us/gov/socket/datagram.h>

using namespace std;





void test_1(int serviceNumber){
	us::gov::socket::datagram d(serviceNumber);
	cout << "\n------Test-1------ "<< endl;
	cout << "-------------------- \n"<< endl;

	//constructor results
	cout << "resize(h) ---d.h----->  " << d.h << endl;
	cout << "d.dend = d.size()---->  " << d.dend << "  " << d.size() << endl;
	cout << "d.size--------------->  " << d.size() << endl;

	//rest of the code--------------------------------------
	if (d.size() == d.h) {cout << "d.size() == datagram::h" << endl;}
	cout << "parse_uint16()--> " << d.parse_uint16() << endl;
	cout << "maxsize---------> " << d.maxsize << endl;
	cout << "service---------> " << d.service << endl;
	cout << "error-----------> " << d.error << endl;
	cout << "compute_hash()--> " << d.compute_hash() << endl;
	cout << "parse_uint16()--> "  << d.parse_uint16() << endl;
	cout << "completed()-----> " << d.completed() << endl;
}



void test_2(int serviceNumber, int payloadNumber){
	us::gov::socket::datagram d(serviceNumber, payloadNumber);
	cout << "\n-----Test-2------ "<< endl;
	cout << "--------------------\n "<< endl;

	//constructor results
	cout << "resize(h+2) --------------->  " << d.h+2 << endl;
	//cout << "(*this)[h]=payload&0xff --->  " << payloadNumber&0xff << endl;	
	//cout << "(*this)[h+1]=payload>>8 --->  " << payloadNumber>>8 << endl; 
	cout << "dend=size() :-------------->  " << d.dend << "  " << d.size() << endl;

	//rest of the code--------------------------------------
	cout << "parse_uint16()--> " << d.parse_uint16() << endl;
	cout << "maxsize---------> " << d.maxsize << endl;
	cout << "service---------> " << d.service << endl;
	cout << "error-----------> " << d.error << endl;
	cout << "compute_hash()--> " << d.compute_hash() << endl;
	cout << "parse_uint16()--> "  << d.parse_uint16() << endl;
	cout << "completed()-----> " << d.completed() << endl;
}




void test_3(int serviceNumber , string payloadString){
	us::gov::socket::datagram d(serviceNumber, payloadString);
	cout << "\n------Test-3------ "<< endl;
	cout << "----------------------- \n"<< endl;

	//constructor results--------------------------------
	cout << "h + payload.size()---------------> " << d.h + payloadString.size() << "   < maxsize: " << d.maxsize  << endl; 
	cout << "resize(h+payload.size())---------> " << d.h + payloadString.size() << endl;
	cout << "h--------------------------------> " << d.h << endl;
	cout << "d.payloadString.size() ----------> " << payloadString.size()<< endl;
	cout << "d.size()------------------------> " << d.size() << endl;
	cout << "dend=size()---------------------> " << d.dend << "  " << d.size() << endl;

	//rest of the code--------------------------------------
	cout << "parse_string()--> " << d.parse_string() << endl;
	cout << "maxsize---------> " << d.maxsize << endl;
	cout << "service---------> " << d.service << endl;
	cout << "error-----------> " << d.error << endl;
	cout << "compute_hash()--> " << d.compute_hash() << endl;
	cout << "parse_uint16()--> "  << d.parse_uint16() << endl;
	cout << "completed()-----> " << d.completed() << endl;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------



//...:::-1-:::....

bool test_data_1(const int& init_num, const int& dend, const int& size,  const int& service , const int& error ,const string& hash ,const int& complete ){ 
	
	us::gov::socket::datagram d(init_num);

	if( d.dend != dend || d.size()!= size || d.service != service || d.error != error ||  d.compute_hash().to_b58() != hash || d.completed() != complete){ 
		assert (false);
	 }
return true;
}


//...:::-2-:::....

bool test_data_2(const int& init_num1,const int& init_num2,  const int& dend, const int& size,  const int& service , const int& error , const int& parse_uint16 , const int& complete){ 
	
	us::gov::socket::datagram d(init_num1, init_num2);
	
	if(  d.dend != dend || d.size()!= size ||  d.service != service || d.error != error || d.parse_uint16()!=parse_uint16 || d.completed() != complete)  {  	 
		assert (false);
	 }
	cout << d.compute_hash() << endl;
return true;
}


//...:::-3-:::....


bool test_data_3(const int& init_num1,const string& init_num2, const int& dend, const int& size, const int& service , const int& error , const int& payload_size , const string& parse_string , const int& complete){ 
	
	us::gov::socket::datagram d(init_num1, init_num2);
	
	if(  d.dend != dend || d.size()!= size || d.service != service || d.error != error || init_num2.size()!=payload_size ||  d.parse_string()!= parse_string || d.completed() != complete)  {  	 
		assert (false);
	 }
	
return true;
}





//......................................................:::::Main Function:::::...........................................	
 void testing_socket_datagram(){

	        //init_num ,dend,size ,service, error,complete
	test_data_1(0      , 6 , 6       ,0       ,0     , "2FMmfVcFZfWMEwbuQsdtu5cSZXWN" , 1  );	
	
	//test_data_1(1,6,6,1,0,1);
 

	//......1...
	//test_1(0);
	//test_1(1);
	//test_1(10);
	//test_1(100);
	//test_1(1000);
	//test_1(13515);



	//init_num1, init_num2 ,dend,size ,service, error,parse_uint16 ,complete
	//test_data_2(0,   0     , 8,   8    , 1,        0  ,    5        ,   1  );

	
	//......2.....
	//test_2(0,0);
	//test_2(1,5);
	//test_2(10,25);
	//test_2(100,105);




	//init_num1, init_num2 ,dend,size,  ,service, error , payload_size , parse_string , complete
	// test_data_3(0, ""     ,  6 , 6    , 0     , 0    ,       0       ,       ""    ,     1  );	

	//......3........
	//test_3(0,"");
	//test_3(0,"z");	
	//test_3(1,"zz");
	//test_3(10,"KKK");
	//test_3(101,"KkKK");

}


