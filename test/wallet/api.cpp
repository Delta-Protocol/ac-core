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
using datagram=us::gov::socket::datagram;




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
		//cout << k.priv.to_b58() << endl;
		//cout << k.pub.to_b58() << endl;
		//cout << k.pub.compute_hash() << endl;
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
		//cout << "connected" << endl;
	}
};



//------------------------------------/--------------------------------------------|





void test(string ip , int port , string address ,int amoundSend, int fees , string privKey , bool timer , bool addToWallet){


string tx_receipt="";
	
	
	test_api ts( ip , port );

//ts.gen_keys();
//api::priv_key(privKey , cout);
//ts.dump(cout);
//ts.list_devices(cout);



try{
	//------add_address--to-wallet---------------|
	if(addToWallet==true){
		ostringstream osAddaddr;
		ts.add_address(crypto::ec::keys::priv_t::from_b58( privKey ) , osAddaddr);
		string addAddr = osAddaddr.str();
		//cout << "Add addr to wallet----> "<< addAddr << "\n" << endl;
	}
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
	sleep(65);
}



//--------------------check--addr--balance-----------------|
try{
	test_client c( 0 );
	c.connect( "localhost", 46001 , false );	//------------------------------------------------------------here-------
	
	string howManyAddr = "1 ";
	string askBalanceAddr = howManyAddr + address;	

	string addrBalance = ts.ask(protocol::cash_query, askBalanceAddr ); 
//	cout << addrBalance << endl;

	int index = 0;
	for (int i = 0; i < 2; ++i){
	    index = (addrBalance.find(" ", index)) + 1; }
	string balance = addrBalance.substr(index); //get only the balance number
	cout << address <<" Balance : " << balance.substr(0, balance.find(" ")) << endl;
} catch (const std::exception& e){assert(false);}




try{
	//----------------new-Address--------------|
	ostringstream os;
	ts.new_address(os);
	string addr = os.str();

	int index = 0;
	for (int i = 0; i < 1; ++i){
	   	index = (addr.find(" ", index)) + 1; }
	string theAddr= addr.substr(index);
	string onlyAddr = theAddr.substr(0, theAddr.find(" "));
	if(onlyAddr.length()==30||onlyAddr.length()==29){  }else{ assert(false);} //"address length is OK \n"
} catch (const std::exception& e){assert(false);}




try{	//------tx_decode----------|
	ostringstream osDecode;
	ts.tx_decode( tx_receipt , osDecode);
	string txdecode = osDecode.str();
	cout << txdecode << endl;

	//-----tx-check-------------|
	ostringstream osCheck;
	ts.tx_check( tx_receipt , osCheck);		
	string txCheck = osCheck.str();
	cout << txCheck << endl;	

	//-----tx-send-------------|
	ostringstream osSend;
	ts.tx_send( tx_receipt , osSend);
	string txSend = osSend.str();
	cout << txSend << endl;
} catch (const std::exception& e){assert(false);}





cout << " \n   --   --   --   --   --   --   --   --   --   --  \n" << endl;
}






void testing_wallet_api()
{

      for (int i=1; i<=3; i++ ) 
      {
	cout << " \n-------------------------------------Test------------------------------------------------\n" << endl;
	//     [ip]       [port]        [address]               [amoundSend]  [fees] 	          [privKey] 			    [timer]  [addToWallet]
	test("127.0.0.1" , 46001 , "9koUJsXUmpJwWb5uzUm3VNwPz6c"  ,  1000   ,   2  , "DUC111C2yiNZiKs9KPTWSTphjMNGHdPN7PeH8j8hvm1m"  , false  ,	false	);
	test("127.0.0.1" , 46001 , "3nuMp9bm19CGag9a5SvJ3A2n8jTi" ,  2000   ,   2  , "5NAnVaY2tHeHMxWk6faPRvWfB7RmCXbxMoqqMsFU7876"  , false  ,	false	);
	test("127.0.0.1" , 46001 , "3U8AUYbfGWkUncvPSNwR9o3Z8BBs" ,  3000   ,   2  , "8WeT9ZJSSaQ1V5XUjJZy9gXVBFhaPsjqhd8ciepiMePc"  , false  ,	false	);
	test("127.0.0.1" , 46001 , "B2D7drjckG2Vdju2GQeYWSA4EEJ"  ,  4000   ,   2  , "Hn6yynQuh4HP3HtmUnP1D3mrsiNThopgzVDxYxY5xU72"  , false  ,	false	);

//	test("127.0.0.1" , 46001 , "9koUJsXUmpJwWb5uzUm3VNwPz6c"  ,  1000   ,   2  , "DUC111C2yiNZiKs9KPTWSTphjMNGHdPN7PeH8j8hvm1m"  , false  ,	true	);
//	test("127.0.0.1" , 46001 , "3nuMp9bm19CGag9a5SvJ3A2n8jTi" ,  2000   ,   2  , "5NAnVaY2tHeHMxWk6faPRvWfB7RmCXbxMoqqMsFU7876"  , false  ,	false	);
//	test("127.0.0.1" , 46001 , "3U8AUYbfGWkUncvPSNwR9o3Z8BBs" ,  3000   ,   2  , "8WeT9ZJSSaQ1V5XUjJZy9gXVBFhaPsjqhd8ciepiMePc"  , false  ,	true	);
//	test("127.0.0.1" , 46001 , "B2D7drjckG2Vdju2GQeYWSA4EEJ"  ,  4000   ,   2  , "Hn6yynQuh4HP3HtmUnP1D3mrsiNThopgzVDxYxY5xU72"  , false  ,	false	);


      }

}




   









/*






			 [private key ]					[Public Key]

	DUC111C2yiNZiKs9KPTWSTphjMNGHdPN7PeH8j8hvm1m  		9koUJsXUmpJwWb5uzUm3VNwPz6c              
	5NAnVaY2tHeHMxWk6faPRvWfB7RmCXbxMoqqMsFU7876		3nuMp9bm19CGag9a5SvJ3A2n8jTi
	8WeT9ZJSSaQ1V5XUjJZy9gXVBFhaPsjqhd8ciepiMePc		3U8AUYbfGWkUncvPSNwR9o3Z8BBs
	Hn6yynQuh4HP3HtmUnP1D3mrsiNThopgzVDxYxY5xU72		B2D7drjckG2Vdju2GQeYWSA4EEJ

*/





