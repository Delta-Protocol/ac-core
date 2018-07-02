#include <iostream>
#include <ostream>
#include <string>
#include <sstream>

#include "api.h"

#include <us/gov/socket/datagram.h>
#include <us/wallet/api.h>
#include <us/wallet/wallet.h>

#include <us/gov/socket/datagram.h>
#include <us/gov/crypto.h>

#include <us/gov/socket/server.h>//---
#include <us/gov/socket/client.h>//--
#include <thread>

using namespace std;

using namespace us::wallet;


using datagram=us::gov::socket::datagram;//socket::datagram;


struct test_api : rpc_api {

	test_api( string walletd_host, uint16_t walletd_port): rpc_api( walletd_host, walletd_port ) {}

 

string ask(int service, const string& args){

	datagram* q=new datagram(service,args);        //(walletd_host, walletd_port, q);
	socket::datagram* response=socket::peer_t::send_recv("127.0.0.1" , 46000 , q); 

	if (response) {
		return response->parse_string();
		delete response;
	}
	else {
		cout << "ERROR" << endl;
	}
}

void gen_keys() {
	crypto::ec::keys k=crypto::ec::keys::generate();
	 k.priv.to_b58();
	 k.pub.to_b58();
	 k.pub.compute_hash();
}

};



using namespace us::gov::socket;


struct test_client: client {
	
	test_client(int sock):client(sock) {
   	}

	virtual void on_connect() override {
		cout << "connected" << endl;
	}
};



//------------------------------------/--------------------------------------------|





void test(string ip , int port , string address ,int amoundSend, int fees , string privKey , bool timer){


string tx_receipt="";
	
	
	test_api ts( ip , port );

	


	ts.gen_keys();


try{
	//------add_address--to-wallet---------------|
	ostringstream osAddaddr;
	//ts.add_address(crypto::ec::keys::priv_t::from_b58( privKey ) , osAddaddr);        --------------------> uncomment <---------------
	string addAddr = osAddaddr.str();
	//cout << "new Wallet address : "<< addAddr << "\n" << endl;
} catch (const std::exception& e){assert(false);}



try{
	//-----------wallet--balance-----------------|
	ostringstream osb;
	ts.balance( true , osb );
	string balance(osb.str());
	cout << "Total Wallet Balance: "<< balance << "\n" << endl;
} catch (const std::exception& e){assert(false);}






try{	
	//---------------tx_make_p2pkh()------------|
	api::tx_make_p2pkh_input i;
	
	ostringstream osTX;

	i.rcpt_addr = address; //--address
	i.amount = amoundSend; //--amoundSend
	i.fee = fees;          //--fees
//	i.sigcode_inputs = 0;
//	i.sigcode_outputs = 1;
//	i.sendover = 1;	
	ts.tx_make_p2pkh(i,osTX);
	tx_receipt = osTX.str();
	//cout << tx_receipt << endl;
}  catch (const std::exception& e){assert(false);}




if (timer != false ){
	sleep(60);
}



//--------------------check-addr-balance-----------------|
try{
	test_client c( 0 );
	c.connect( "localhost", 46001 , false );	
	

	string addrBalance = ts.ask(protocol::cash_query, "1 "+address); //----------------------------------------------------here-------
	int index = 0;
	for (int i = 0; i < 2; ++i){
	    index = (addrBalance.find(" ", index)) + 1; }
	string balance= addrBalance.substr(index);
	cout << balance.substr(0, balance.find(" ")) << endl;
} catch (const std::exception& e){assert(false);}






try{
	//----------------new-Address--------------|
	ostringstream os;
	ts.new_address(os);
	string addr = os.str();
	if(addr.length()==30||addr.length()==29){}else{ assert(false);} //"address length is OK \n"
} catch (const std::exception& e){assert(false);}

	


try{	//------tx_decode----------|
	ostringstream osDecode;
	ts.tx_decode( tx_receipt , osDecode);
	string txdecode = osDecode.str();
	//cout << txdecode << endl;

	//-----tx-check-------------|
	ostringstream osCheck;
	ts.tx_check( tx_receipt , osCheck);		
	string txCheck = osCheck.str();
	//cout << txCheck << endl;	

	//-----tx-send-------------|
	ostringstream osSend;
	ts.tx_send( tx_receipt , osSend);
	string txSend = osSend.str();
	//cout << txSend << endl;
} catch (const std::exception& e){assert(false);}

cout << "    --   --   --   --   --   --   --   --   --   --  \n" << endl;

}






void testing_wallet_api()
{

	for (int i=1; i<=3; i++ ) 
	{
	cout << " ---------------------------Test-------------------------------" << endl;
		//     [ip]       [port]        [address]               [amoundSend]  [fees] 	[privateKey] 			            [timer]
		test("127.0.0.1" , 46001 , "4WFcdMLaDEG4WPE8pHCTLZymBneJ" ,  10000   ,   1 , "CMkbepKCiVLnqt8XwYM3KWxDzhRZqecoYyL7ZXG75ZFu" , true );
	sleep(20);		
		test("127.0.0.1" , 46001 , "3onaAM6pyGMx5FyA1VuYADpcH6wG" ,  20000   ,   1 , "CMkbepKCiVLnqt8XwYM3KWxDzhRZqecoYyL7ZXG75ZFu" , true  );
	sleep(20);		
		test("127.0.0.1" , 46001 , "2f4M2qtWznu3PrSm3qqM9HS4SUJD" ,  30000   ,   1 , "CMkbepKCiVLnqt8XwYM3KWxDzhRZqecoYyL7ZXG75ZFu" , true );
//		test("127.0.0.1" , 46001 , "29zPdkeU82Fzp8QRrNQVocqPKpqZ" ,  40000   ,   1 , "CMkbepKCiVLnqt8XwYM3KWxDzhRZqecoYyL7ZXG75ZFu" , true );
//		test("127.0.0.1" , 46001 , "2HfujHeDRWX4qCM35c3UqtXaFDxd" ,  55555   ,   1 , "CMkbepKCiVLnqt8XwYM3KWxDzhRZqecoYyL7ZXG75ZFu" , true );
//		test("127.0.0.1" , 46001 , "4TQWGGFjoqRvifGsp6hKBTcsw4x6" ,  66666   ,   1 , "CMkbepKCiVLnqt8XwYM3KWxDzhRZqecoYyL7ZXG75ZFu" , true );
	}

}









   









/*

	//ts.list_devices(cout);

	//ts.dump(cout);	

	//api::priv_key("5k6K4uBmM86rKYQZqytctexBwfwN4GeDvvVZgBXxtbE9",cout); 

*/





