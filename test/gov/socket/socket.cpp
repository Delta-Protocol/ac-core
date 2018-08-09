#include <iostream>
#include <string>
#include "socket.h"
#include <us/gov/socket/datagram.h>

#include <us/gov/socket/server.h>
#include <us/gov/socket/client.h>//--
#include <us/gov/blockchain/daemon.h>

#include <thread>

#include <vector>

using namespace std;


     
void test_1(int serviceNumber){
	us::gov::socket::datagram d(serviceNumber);
	cout << "\n|------------------------------------------------------------| "	<< endl;
	cout << "|--------------------------Test-1----------------------------| "	<< endl;
	cout << "|------------------------------------------------------------| \n"	<< endl;

	//constructors results---------------------------
	cout << "resize(h) ---d.h----->  " << d.h << endl;
	cout << "d.dend = d.size()---->  " << d.dend << " - " << d.size() << endl;
	cout << "d.size--------------->  " << d.size() << endl;

	//rest of the code-----------------------------------------------
	if (d.size() == d.h) {cout << "d.size() == datagram::h" << endl;}
	cout << "maxsize---------> " << d.maxsize << endl;
	cout << "service---------> " << d.service << endl;
	//cout << "error-----------> " << d.error << endl;
	cout << "compute_hash()--> " << d.compute_hash() << endl;
	cout << "parse_uint16()--> "  << d.parse_uint16() << endl;
	cout << "completed()-----> " << d.completed() << endl;
}


void test_2(int serviceNumber, int payloadNumber){
	us::gov::socket::datagram d(serviceNumber, payloadNumber);
	cout << "\n|------------------------------------------------------------| "	<< endl;
	cout << "|--------------------------Test-2----------------------------| "	<< endl;
	cout << "|------------------------------------------------------------| \n"	<< endl;

	//constructors results--------------------------------------
	cout << "resize(h+2) ----------------->  " << d.h+2  << endl;
	cout << "dend=size() ----------------->  " << d.dend << " - " << d.size() << endl;

	//rest of the code--------------------------------
	cout << "maxsize---------> " << d.maxsize << endl;
	cout << "service---------> " << d.service << endl;
	//cout << "error-----------> " << d.error << endl;
	cout << "compute_hash()--> " << d.compute_hash() << endl;
	cout << "parse_uint16()--> " << d.parse_uint16() << endl;
	cout << "completed()-----> " << d.completed()    << endl;
}


void test_3(int serviceNumber , string payloadString){
	us::gov::socket::datagram d(serviceNumber, payloadString);
	cout << "\n|------------------------------------------------------------| "	<< endl;
	cout << "|--------------------------Test-3----------------------------| "	<< endl;
	cout << "|------------------------------------------------------------| \n"	<< endl;

	//constructors results------------------------------------------------------------------------------------------------
	cout << "h + payload.size()---------------> " << d.h + payloadString.size() << "   < maxsize: " << d.maxsize  << endl; 
	cout << "resize(h+payload.size())---------> " << d.h + payloadString.size() << endl;
	cout << "h--------------------------------> " << d.h 			    << endl;
	cout << "d.payloadString.size() ----------> " << payloadString.size()       << endl;
	cout << "d.size()-------------------------> " << d.size()                   << endl;
	cout << "dend=size()----------------------> " << d.dend << " - "<< d.size() << endl;

	//rest of the code-------------------------------------------------------------------
	cout << "parse_string()--> " << "\""             << d.parse_string() << "\"" << endl;
	cout << "maxsize---------> " << d.maxsize 	 << endl;
	cout << "service---------> " << d.service 	 << endl;
	//cout << "error-----------> " << d.error          << endl;
	cout << "compute_hash()--> " << d.compute_hash() << endl;
	cout << "parse_uint16()--> " << d.parse_uint16() << endl;
	cout << "completed()-----> " << d.completed()    << endl;
}

//-------------------------------|

class TestDatagram {
   public:
	TestDatagram(){}
	
	TestDatagram(int svc):d(svc) {}
	TestDatagram(int svc, int init_num):d(svc,init_num) {}
	TestDatagram(int svc, string init_string):d(svc,init_string) {}
	
	us::gov::socket::datagram d;
  
      bool test_data(const int& dend, const int& size,  const int& service , const string& hash ,const int& complete ) //const int& error ,
	{ 
		if( d.dend != dend || d.size()!= size || d.service != service ||  d.compute_hash().to_b58() != hash || d.completed() != complete) //d.error != error || 
		{ 
			assert (false);
		}
	return true;
 	}
};


class Uint16 {
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


class TestPayloadString {
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

	
			
};





void testing_socket_datagram(){


//---1--------------------------------------------------------------------------------------------|
//       [serviceNumber]
//	 	          [ dend ],[ size ],[ service ],[ error ],[ hash ],[ complete ]
	TestDatagram a(0);
	a.test_data( 6  ,  6   ,   0    , "2FMmfVcFZfWMEwbuQsdtu5cSZXWN"  ,  1);
	
	TestDatagram b(1);
	b.test_data( 6  ,  6   ,   1   ,    "3Z28rsteneYa98hvxR2ReLxP9jW"  ,  1);		

	TestDatagram c(10);
	c.test_data( 6  ,  6   ,   10  ,   "N5V1hPddzeWtKbTHECoPEYBijn6"  ,   1);

	TestDatagram d(100);
	d.test_data( 6  ,  6   ,   100 ,   "2fJEMgiNAhGwa6SPD8MK3wyhDCn8"  ,  1);

	TestDatagram e(1000);
	e.test_data( 6  ,  6   ,   1000,  "3eHUfaBJoEDae8oSvHwemt2vraMB"  ,   1);

	TestDatagram f(65535);
	f.test_data( 6  ,  6   ,  65535,   "3xperfdBSJ6cthKMQLN5Mwa2jdJ5"  ,  1);

	TestDatagram g(65536);
	g.test_data( 6  ,  6   ,    0  ,   "2FMmfVcFZfWMEwbuQsdtu5cSZXWN"  ,  1);



//---2---------------------------------------------------------------------------------------------|
//       [serviceNumber][ payloadNumber ]
//	 	          [ dend ],[ size ],[ service ],[ error ],[ hash ],[ complete ]

	TestDatagram a2(0,0);
	a2.test_data(8  ,   8   ,  0    ,    "32idzgT8tKQT5yxjiGiGyufpSqXB"  ,  1 );
        Uint16 a2b(0,0);
        a2b.test_uint16(0);
	
	TestDatagram b2(1,2);
	b2.test_data(8  ,   8   ,  1    ,   "3FYZb9H4K2tkzZUgypqGAFnoPfAr"  ,   1 );
        Uint16 b2b(1,2);
	b2b.test_uint16(2);

	TestDatagram c2(100,0);
	c2.test_data(8  ,   8   ,  100  ,   "2VpyaUEEgz4Yo38hX1MvvrRpFkgb"  ,   1 );
	Uint16 c2b(100,0);
	c2b.test_uint16(0);
	
	TestDatagram d2(1000,1000);
	d2.test_data(8  ,   8   ,  1000 ,   "2D2PNV2j1y7xjjNiKJ3YvqtsbZTR"  ,   1 );
	Uint16 d2b(1000,1000);
	d2b.test_uint16(1000);
	
	TestDatagram e2(65536,65535);
	e2.test_data(8  ,   8   ,  0    ,  "1BrDi8KcT7ocoCx4rJhYEXRmGFX"  ,    1 );
	Uint16 e2b(65536,65535);
        e2b.test_uint16(65535);
	
	TestDatagram f2(65536,65536);
	f2.test_data(8  ,   8   ,  0    ,  "32idzgT8tKQT5yxjiGiGyufpSqXB"  ,   1 );
	Uint16 f2b(65536,65536);
        f2b.test_uint16(0);
	
	TestDatagram g2(0,65536);
	g2.test_data(8  ,   8   ,  0    ,  "32idzgT8tKQT5yxjiGiGyufpSqXB"  ,   1 );
        Uint16 g2b(0,65536);
        g2b.test_uint16(0);



//---3--------------------------------------------------------------------------------------------|
//       [serviceNumber][ payloadString ] 
//	 	          [ dend ],[ size ],[ service ],[ error ],[ hash ],[ complete ]

	TestDatagram a3(0, "zz");
	a3.test_data( 8 , 8    , 0     , "7cU4a99HHJWAD5grBsjYAVsXqJW",1);
	TestPayloadString a3b(0, "zz");         
	a3b.test_payloadString(2, "zz");
	
	TestDatagram b3(100, "");
	b3.test_data( 6 , 6    , 100    , "2fJEMgiNAhGwa6SPD8MK3wyhDCn8",1);
	TestPayloadString b3b(100,  "");         
	b3b.test_payloadString(0, "");

	TestDatagram c3(65537, "");
	c3.test_data( 6 , 6    , 1     , "3Z28rsteneYa98hvxR2ReLxP9jW",1);
	TestPayloadString c3b(65537, "");         
	c3b.test_payloadString(0, "");

	TestDatagram d3(65536, "");
	d3.test_data( 6 , 6    , 0      , "2FMmfVcFZfWMEwbuQsdtu5cSZXWN",1);
	TestPayloadString d3b(65536, "");         
	d3b.test_payloadString(0, "");

	TestDatagram e3(0, "abcdefghijklmnop");
	e3.test_data( 22 , 22    , 0     , "4en91amD8tQhTYmyYbL2doWYDJB",1);
	TestPayloadString e3b(0, "abcdefghijklmnop");         
	e3b.test_payloadString(16, "abcdefghijklmnop");

	TestDatagram f3(111, "a");
	f3.test_data( 7 , 7    , 111     ,"yydr76MzZdEERbHnWth2udEgZ99",1);
	TestPayloadString f3b(111, "a");         
	f3b.test_payloadString(1, "a");

	TestDatagram g3(65536, "0");
	g3.test_data( 7 , 7    , 0     , "fC8m731SsLTfdfZR4XWQ8Goupq5",1);
	TestPayloadString g3b(65536, "0");         
	g3b.test_payloadString(1, "0");

	TestDatagram h3(13564, "123-)(*&^%$£!`~@'#?><");
	h3.test_data( 28 , 28    , 13564     , "t6mPNfYBGivpDLuU3u5SwB9i8uj",1);
	TestPayloadString h3b(13564, "123-)(*&^%$£!`~@'#?><");         
	h3b.test_payloadString(22, "123-)(*&^%$£!`~@'#?><");		
	
	TestDatagram i3(52992, "123456\n78  _  _");
	i3.test_data( 21 , 21    , 52992     , "HmRjW5K1LBfMfp5VeULhoWpMnj2",1);
	TestPayloadString i3b(52992, "123456\n78  _  _");         
	i3b.test_payloadString(15, "123456\n78  _  _");

	TestDatagram j3(-2140000000, "0_+_)(*&0*(0^$%^^_0");
	j3.test_data( 25 , 25    , 12544     , "KxxdXgH2J61Kjscsz3rY2Dbgwxn",1);
	TestPayloadString j3b(-2140000000, "0_+_)(*&0*(0^$%^^_0");         
	j3b.test_payloadString(19, "0_+_)(*&0*(0^$%^^_0");

	TestDatagram k3(-1, "miden 0 miden");
	k3.test_data( 19 , 19    , 65535     , "2RsgjojwWtgmaS8W7Mee9gzCqaFR",1);
	TestPayloadString k3b(-1, "miden 0 miden");         
	k3b.test_payloadString(13, "miden 0 miden");


	TestDatagram l3(-100, "0 0--%^*& (*&^6-0-75gj00 ");
	l3.test_data( 31 , 31    , 65436     , "26AMHWW1fTm65x7oBwEHpUpwTVCQ",1);
	TestPayloadString l3b(-100,  "0 0--%^*& (*&^6-0-75gj00 ");         
	l3b.test_payloadString(25, "0 0--%^*& (*&^6-0-75gj00 ");


	TestDatagram m3(2147483647, " 0 ");
	m3.test_data( 9 , 9    , 65535     , "juhNKRux67Uv4ji2k9dFMnmVSir",1);
	TestPayloadString m3b(2147483647,  " 0 " );         
	m3b.test_payloadString( 3 , " 0 ");


	TestDatagram n3(-2147483647, "0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 ");
	n3.test_data( 170 , 170    , 1     , "4UNpGamqVspG9ztPHwK9JC4dt2YV",1);
	TestPayloadString n3b(-2147483647,  "0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 " );         
	n3b.test_payloadString( 164 , "0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 ");

//---

	TestDatagram o3(-23647 , "0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 ");
	o3.test_data( 1063 , 1063    , 41889     , "2dWbqRnHzVtQe9jeatuk6Pf9D2Ee",1);
	TestPayloadString o3b(-23647,  "0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 " );         
	o3b.test_payloadString( 1057 , "0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 ");

//------

	TestDatagram p3(-123456789, "0 0 0 gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO 0 0 0");
	p3.test_data( 1792 , 1792    , 13035     , "iomD1gz3ZS6kdi7QdrEAmDQiFyp",1);
	TestPayloadString p3b(-123456789,  "0 0 0 gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO 0 0 0" );         
	p3b.test_payloadString( 1786 , "0 0 0 gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO 0 0 0");

//------


//	...:::HELLO::TESTER::::... ---> change to "true" if you want to see test_1 , test_2  , test_3 --> output 
	bool showResults = false; 
	


	if( showResults == true ) {
	//......1...............//[serviceNumber]
	test_1(0);  
	test_1(1);
	test_1(10);
	test_1(100);
	test_1(1000);
	test_1(13515);

	//......2...............// [ serviceNumber ] [ payloadNumber ]
	test_2( 0     , 0     ); 
	test_2( 1     , 2     );
	test_2( 100   , 0     );
	test_2( 1000  , 1000  );
	test_2( 65536 , 65535 );
	test_2( 65536 , 65536 );
	test_2( 0     , 65536 );

	test_2( 03     , 04 );

	//......3...............// [ serviceNumber ] [ payloadString ]
	test_3( 0     , "zz"  );   
	test_3( 100   , ""    );	
	test_3( 65537 , ""    );
	test_3( 65536 , ""    );
	test_3( 0     , "abcdefghijklmnop");
	test_3( 111   , "a"   );
	test_3( 65536 , "0"   );
	test_3( 13564 , "123-)(*&^%$£!`~@'#?><"    );
	test_3( 2147483648 , "123456\n78  _  _"	   ); //  overflow:  > 2147483648 || -2147483648
	test_3( -2147483648 , "0_+_)(*&0*(0^$%^^_0");  
	test_3( -1      , "miden 0 miden"          );
	test_3( -100   ,"0 0--%^*& (*&^6-0-75gj00 ");
	test_3( 2147483647   ,  " 0 "     	   );
	test_3( -2147483647   ,  "0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 " );	
	test_3( -23647   ,  "0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 000000 0000000 00000 0 0000 00 000000 000000 000000 0000000 00000000 0000000 000 000 000 0 0 0 000  000 00000 000000  0000 00000 000000 00000 00000 0000000 0000000 " );

	test_3( -123456789   ,  "0 0 0 gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO  gkjfhdglkjfdhgariudhfdkjhgfdkjhy76^&%*^&$%FGVHKJGFV  r769trewrteorf2102401685710retg0r5460t9876fdjhgGFGJFR TO 0 0 0" );

}


}




//---------------------------------------------testing_socket_communication()---------------------------------------------------|

using namespace us::gov::socket;


struct test_client: client {
	
	test_client(int sock):client(sock) {}

	virtual void on_connect() override {
		//cout << "connected" << endl;
	}
};

//--------------------------------------------------|

struct test_server: server {

	thread t;
	
	test_server(uint16_t port): server(port){ } 
	
	~test_server() { }


//	void receive_and_process(client* scl) override{	}

//	void process_work(peer_t *c) override{ }


	
/*	bool receive_and_process(client*c) override {		// us/gov/socket/daemon
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
		//delete d;                                       
		//server::(c);
	}*/



	 //virtual void check_received_datagram(datagram* data)=0;
	 //virtual void check_received_datagram(datagram* data); 



	client* create_client(int sock) override {
		return new test_client(sock);	}
};




//--------------------------------------------------------------|

	void run_server(int port) {	
		test_server kk(port);
		kk.make_socket(port);
		kk.run();
	}

	void send_vectors(datagram d, int port)
	{
		test_client c( port );
	 	c.init_sock("localhost", port , false );	
		c.connect(  "localhost", port , false );

		c.send(d);
	}

	void run(vector<datagram> k){
		 for (int i = 0; i < k.size(); i++)
      		{
			send_vectors(k[i] , 1065);
		}
	}

//--------------------------------------------------------------|




#include<chrono>
using namespace std::chrono_literals;

void testing_socket_communication(){



//	-1-  Put "noise" inside the vector of datagrams

	vector<datagram> k;
	k.push_back( us::gov::socket::datagram( 0 ,"yeah") );
	k.push_back( us::gov::socket::datagram( 1 , 125  ) );
	k.push_back( us::gov::socket::datagram( 10 ,"some random string 0x60 yeah  ah") );
	k.push_back(48);
	k.push_back(239);
	k.push_back(32);
	k.push_back(123);
	k.push_back(0xF);
	k.push_back(239);
	k.push_back(32);
	k.push_back(123);
	k.push_back(0xF);
	k.push_back(48);
	k.push_back(0xF);
	k.push_back(0xC);
	k.push_back(48);
	k.push_back(0xF);
	k.push_back(0xC);
	k.push_back(0xfd);
	k.push_back( us::gov::socket::datagram( 100 ," fluhs  0xF  0xF  0xF df  0xF  fd ") );
	k.push_back( us::gov::socket::datagram( 1000 ,"yeah") );
	k.push_back( us::gov::socket::datagram( 10000 , 125  ) );
	k.push_back( us::gov::socket::datagram( 12356 ,"some random string 0x60 yeah  ah") );


//	-2- run server thread

	std::thread t(run_server, 1065);
	this_thread::sleep_for(100ms);


//	-3- send vector of datagrams

	run(k);


//	-4- server thread detach

	t.detach();



	//			MUST DO - check -> struct test_server{ }
	//************************************************************************ 
	//	 1. catch completed datagram ----------------------------------> *  --?????-- 		
	//	 2. test the complete datagram --------------------------------> *  	--????--
	//************************************************************************ 




}






