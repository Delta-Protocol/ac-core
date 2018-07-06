#include <iostream>
#include <string>
#include "socket.h"//testing socket h
#include <us/gov/socket/datagram.h>

#include <us/gov/socket/server.h>//---
#include <us/gov/socket/client.h>//--
#include <us/gov/blockchain/daemon.h>
#include <thread>

using namespace std;

/*             uncomment if you want to check datagrams results

     
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
*/
/*
void test_2(int serviceNumber, int payloadNumber){
	us::gov::socket::datagram d(serviceNumber, payloadNumber);
	cout << "\n-----Test-2------ "<< endl;
	cout << "--------------------\n "<< endl;

	//constructor results
	cout << "resize(h+2) ----------------->  " << d.h+2 << endl;
	//cout << "(*this)[h]=payload&0xff --->  " << payloadNumber&0xff << endl;	
	//cout << "(*this)[h+1]=payload>>8 --->  " << payloadNumber>>8 << endl; 
	cout << "dend=size() :---------------->  " << d.dend << "  " << d.size() << endl;

	//rest of the code--------------------------------------
	cout << "parse_uint16()--> " << d.parse_uint16() << endl;
	cout << "maxsize---------> " << d.maxsize << endl;
	cout << "service---------> " << d.service << endl;
	cout << "error-----------> " << d.error << endl;
	cout << "compute_hash()--> " << d.compute_hash() << endl;
	cout << "parse_uint16()--> "  << d.parse_uint16() << endl;
	cout << "completed()-----> " << d.completed() << endl;
}
*/
/*
void test_3(int serviceNumber , string payloadString){
	us::gov::socket::datagram d(serviceNumber, payloadString);
	cout << "\n------Test-3------ "<< endl;
	cout << "----------------------- \n"<< endl;

	//constructor results--------------------------------
	cout << "h + payload.size()---------------> " << d.h + payloadString.size() << "   < maxsize: " << d.maxsize  << endl; 
	cout << "resize(h+payload.size())---------> " << d.h + payloadString.size() << endl;
	cout << "h--------------------------------> " << d.h << endl;
	cout << "d.payloadString.size() ----------> " << payloadString.size()<< endl;
	cout << "d.size()-------------------------> " << d.size() << endl;
	cout << "dend=size()----------------------> " << d.dend << "  " << d.size() << endl;

	//rest of the code--------------------------------------
	cout << "parse_string()--> " << d.parse_string() << endl;
	cout << "maxsize---------> " << d.maxsize << endl;
	cout << "service---------> " << d.service << endl;
	cout << "error-----------> " << d.error << endl;
	cout << "compute_hash()--> " << d.compute_hash() << endl;
	cout << "parse_uint16()--> "  << d.parse_uint16() << endl;
	cout << "completed()-----> " << d.completed() << endl;
}
*/



class TestDatagram {
   public:
	TestDatagram(){}
	
	TestDatagram(int svc):d(svc) {}
	TestDatagram(int svc, int init_num):d(svc,init_num) {}
	TestDatagram(int svc, string init_string):d(svc,init_string) {}
	
	us::gov::socket::datagram d;
  
      bool test_data(const int& dend, const int& size,  const int& service , const int& error ,const string& hash ,const int& complete )
	{ 
		if( d.dend != dend || d.size()!= size || d.service != service || d.error != error ||  d.compute_hash().to_b58() != hash || d.completed() != complete)
		{ 
			assert (false);
		}
	return true;
 	}
};


class Uint16 {//: public TestDatagram {
   public:

	Uint16(int svc, int init_num):d(svc,init_num) {}
	
	us::gov::socket::datagram d;

	bool test_uint16(const int& parse_uint16)
	{ 
		if( d.parse_uint16()!= parse_uint16)
		{ 
			assert (false);
		}
	return true;
	}	
};


class TestPayloadString {//: public TestDatagram {
   public:

	TestPayloadString(int svc, string init_string):d(svc,init_string) ,payload(init_string){}
	
		

	us::gov::socket::datagram d;
	string payload;

	bool test_payloadString(const size_t& payload_size , const string& parse_string)
	{ 
		if( payload.size()!=payload_size ||  d.parse_string()!= parse_string)
		{ 
			assert (false);
		}
	return true;
	}

	//using TestDatagram::TestDatagram(int svc, string init_string):d(svc,init_string) {}
			
};




 
void testing_socket_datagram(){


//---1--------------------------------------------------------------------------------------------
//	         int_num  |dend |size | service | error|   	     hash             |  complete
	TestDatagram a(0);
		a.test_data( 6  ,  6   ,   0   ,   0  , "2FMmfVcFZfWMEwbuQsdtu5cSZXWN"  ,  1);
	TestDatagram b(1);
		b.test_data( 6  ,  6   ,   1   ,   0  , "3Z28rsteneYa98hvxR2ReLxP9jW"  ,   1);		
	TestDatagram c(10);
		c.test_data( 6  ,  6   ,   10  ,   0  , "N5V1hPddzeWtKbTHECoPEYBijn6"  ,   1);
	TestDatagram d(100);
		d.test_data( 6  ,  6   ,   100 ,   0  , "2fJEMgiNAhGwa6SPD8MK3wyhDCn8"  ,  1);
	TestDatagram e(1000);
		e.test_data( 6  ,  6   ,   1000,   0  , "3eHUfaBJoEDae8oSvHwemt2vraMB"  ,  1);
	TestDatagram f(65535);
		f.test_data( 6  ,  6   ,  65535,   0  , "3xperfdBSJ6cthKMQLN5Mwa2jdJ5"  ,  1);
	TestDatagram g(65536);
		g.test_data( 6  ,  6   ,    0  ,   0  , "2FMmfVcFZfWMEwbuQsdtu5cSZXWN"  ,  1);

//---2---------------------------------------------------------------------------------------------
//	                  int_num  |dend |size | service | error|   	     hash       |  complete
	TestDatagram a2(0,0);
		a2.test_data(8  ,   8   ,  0    ,   0  , "32idzgT8tKQT5yxjiGiGyufpSqXB"  ,   1 );
        		Uint16 a2b(0,0);
               			a2b.test_uint16(0);
	TestDatagram b2(1,2);
		b2.test_data(8  ,   8   ,  1    ,   0  , "3FYZb9H4K2tkzZUgypqGAFnoPfAr"  ,   1 );
		        Uint16 b2b(1,2);
               			b2b.test_uint16(2);
	TestDatagram c2(100,0);
		c2.test_data(8  ,   8   ,  100  ,   0  , "2VpyaUEEgz4Yo38hX1MvvrRpFkgb"  ,   1 );
        		Uint16 c2b(100,0);
               			c2b.test_uint16(0);
	TestDatagram d2(1000,1000);
		d2.test_data(8  ,   8   ,  1000 ,   0  , "2D2PNV2j1y7xjjNiKJ3YvqtsbZTR"  ,   1 );
        		Uint16 d2b(1000,1000);
               			d2b.test_uint16(1000);
	TestDatagram e2(65536,65535);
		e2.test_data(8  ,   8   ,  0    ,   0  , "1BrDi8KcT7ocoCx4rJhYEXRmGFX"  ,   1 );
        		Uint16 e2b(65536,65535);
               			e2b.test_uint16(65535);
	TestDatagram f2(65536,65536);
		f2.test_data(8  ,   8   ,  0    ,   0  , "32idzgT8tKQT5yxjiGiGyufpSqXB"  ,   1 );
        		Uint16 f2b(65536,65536);
               			f2b.test_uint16(0);
	TestDatagram g2(0,65536);
		g2.test_data(8  ,   8   ,  0    ,   0  , "32idzgT8tKQT5yxjiGiGyufpSqXB"  ,   1 );
        		Uint16 g2b(0,65536);
               			g2b.test_uint16(0);

//---3---------------------------------------------------------------------------------------------
//	         int_num  |dend |size | service | error|   	     hash             |  complete
	TestDatagram a3(0, "zz");
		a3.test_data( 8 , 8    , 0     ,0,"7cU4a99HHJWAD5grBsjYAVsXqJW",1);
			TestPayloadString a3b(0, "zz");         
				a3b.test_payloadString(2, "zz");
	TestDatagram b3(100, "");
		b3.test_data( 6 , 6    , 100    ,0,"2fJEMgiNAhGwa6SPD8MK3wyhDCn8",1);
			TestPayloadString b3b(100,  "");         
				b3b.test_payloadString(0, "");
	TestDatagram c3(65537, "");
		c3.test_data( 6 , 6    , 1     , 0,"3Z28rsteneYa98hvxR2ReLxP9jW",1);
			TestPayloadString c3b(65537, "");         
				c3b.test_payloadString(0, "");
	TestDatagram d3(65536, "");
		d3.test_data( 6 , 6    , 0      ,0,"2FMmfVcFZfWMEwbuQsdtu5cSZXWN",1);
			TestPayloadString d3b(65536, "");         
				d3b.test_payloadString(0, "");
	TestDatagram e3(0, "abcdefghijklmnop");
		e3.test_data( 22 , 22    , 0     ,0,"4en91amD8tQhTYmyYbL2doWYDJB",1);
			TestPayloadString e3b(0, "abcdefghijklmnop");         
				e3b.test_payloadString(16, "abcdefghijklmnop");
	TestDatagram f3(111, "a");
		f3.test_data( 7 , 7    , 111     ,0,"yydr76MzZdEERbHnWth2udEgZ99",1);
			TestPayloadString f3b(111, "a");         
				f3b.test_payloadString(1, "a");
	TestDatagram g3(65536, "0");
		g3.test_data( 7 , 7    , 0     ,0,"fC8m731SsLTfdfZR4XWQ8Goupq5",1);
			TestPayloadString g3b(65536, "0");         
				g3b.test_payloadString(1, "0");


	//......1...
	//test_1(0);
	//test_1(1);
	//test_1(10);
	//test_1(100);
	//test_1(1000);
	//test_1(13515);

	//......2.....
	//test_2(0,0);
	//test_2(1,2);
	//test_2(100,0);
	//test_2(1000,1000);
	//test_2(65536,65535);
	//test_2(65536,65536);
	//test_2(0,65536);

	//......3........
	/*test_3(0,"zz");
	test_3(100,"");	
	test_3(65537,"");
	test_3(65536,"");
	test_3(0,"abcdefghijklmnop");
	test_3(111,"a");
	test_3(65536,"0");*/
}




using namespace us::gov::socket;


struct test_client: client {
	
	test_client(int sock):client(sock) {
   	}

	virtual void on_connect() override {
		cout << "connected" << endl;
	}
};


struct test_server: server {

	test_server(uint16_t port): server(port){} 
	
		
	bool receive_and_process(client*c) override {
		datagram* d = c-> complete_datagram();
		if (!d || d->error!=0) {
			cout << "socket: daemon: error recv datagram. clients.remove(fd " << c->sock << ") " << endl;
			if (d) delete d;
			server::receive_and_process(c);
			return true;
			}
		if (!d->completed()) { 
			cout << "socket: daemon: recv partial datagram. returning to listen pool" << endl;
			server::receive_and_process(c);
			return true;
			}

		//---Completed datagram---
			

		//check_received_datagram(*d);
		delete d;                                       
		server::receive_and_process(c);
	}

	virtual void check_received_datagram(datagram* data)=0;

	client* create_client(int sock) override {
		return new test_client(sock);
		}

};



			// class      datagram
struct tests:vector<pair<TestDatagram,datagram>>, test_server {
	
	typedef pair<TestDatagram,datagram> td;
	thread t;
	
	//---run server thread---
	tests():test_server(1060) {

	//	thread t(&server::run, &s);//----------------------> 	error
	//	this_thread::sleep_for(100ms);

	}

	~tests() {
		t.join();
	}


	void test(td& d)
	{
		//---client connected with server---
		test_client c( 1060 );
		c.connect( "localhost", 1060 , false );
		//current = &d;
		//c.send(d.second);  //-------------------------->  error
	}

	
	td* current;

	virtual void check_received_datagram(datagram* data) { //datagram& data) override {
		datagram* d = data;
		//datagram* d = d-> complete_datagram();

		current->first.test_data(d->dend, d->size(), d->service, d->error, d->compute_hash().to_b58(), d->completed());	
	}


	void run(){
		for (auto&d:*this) {
		test(d);
		}
	}
};




#include<chrono>
using namespace std::chrono_literals;

void testing_socket_communication(){

//	tests i(s);
//	i.push_back(pair<datagram,TestDatagram>(TestDatagram(100,12), datagram(100,12)));
/*	i.push_back(pair<datagram,TestDatagram>(TestDatagram(100,12), datagram(100,12)));
	i.push_back(pair<datagram,TestDatagram>(TestDatagram(100,12), datagram(100,12)));
*/

//	i.run();

}





