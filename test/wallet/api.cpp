#include <iostream>
#include <ostream>
#include "api.h"

#include <us/gov/socket/datagram.h>
#include <us/wallet/api.h>
#include <us/wallet/wallet.h>

#include <us/gov/socket/datagram.h>
#include <us/gov/crypto.h>
#include <sstream>

using namespace std;

using namespace us::wallet;



struct test_api : rpc_api {

	test_api( string walletd_host, uint16_t walletd_port): rpc_api( walletd_host, walletd_port ) {}

/*
void  priv_key(const crypto::ec::keys::priv_t& privkey, ostream&os)  {
	if (!crypto::ec::keys::verify(privkey)) {
		os << "The private key is incorrect." << endl;
	}
	auto pub=crypto::ec::keys::get_pubkey(privkey);
	os << "Public key: " << pub << endl;
	os << "Public key hash: " << pub.compute_hash() << endl;
}
*/
};


void testing_wallet_api()
{

	test_api ts( "127.0.0.1" , 46001 );

	ts.balance( true , cout );


//-------new_address()------------|
	ostringstream os;

	ts.new_address(os);

	string addr=os.str();

	//cout << "Length: " << addr.length() << " ---> " << addr << endl;
//------------------------------|

	//ts.dump(cout);

//-------tx_make_p2pkh()--------------------------------|
	api::tx_make_p2pkh_input i;

	i.rcpt_addr = "3dsS2paq5J3c9tW33unQ82jcQmU3";
	i.amount = 33333333;
	i.fee = 11;
//	i.sigcode_inputs = 0;
//	i.sigcode_outputs = 1;
//	i.sendover = 1;	
	//ts.tx_make_p2pkh(i,cout);					
//-----------------------------------------------------|

	//ts.tx_decode(" APej2rnDJxDjzrkXrruHSiHKeK3TfkdbZkKjVNme2j2XxgrZCsYKWZruK9DhG6H9ybnv6csnLT5ARaA2acoX9QYp9scuf6Ph6LyvzhPKk5L6E3gVaxCn193EEZc51kU1b5jFjL8nWZSDp2YEDwSEP2CzM9PYTXLSetK294UXQfRP3c3xvnYkCkiXvcUNSCbiidS79H9PtbF7aXQGK4vMsQgb2W3vub1YYU68GBXuk2jm7v45RRG4oSZ42vcbpx9EbW8v3DicfgN416kFu8s6gYQpF3DJNc4Jrysr6hMyZJiNLvFr7jn1mvS3kgZ6WBxSrciwBwiUyhChpCiVbTC8xeZpadtFdeLpMAQCchbrHopG1ZzqbW1Qr9jKDz6yJ9zFgX5yWAVLhQLF2iy9CJeQUAuk4rD5iJh3y6qsLS21iqmHGGKUPEMvnzyHnvkNKeowBGUKnUsQigDp4a3" , cout);		


	//ts.tx_check(" KrjuM9X2UsU3DFa9FbRRZQyZVdpoaiEJLKzf1Ki8aL6uuXmtfSRWN4gUEfeidHdkzYp3dkL9XjEECrLRygd4rn3YG6wtjuVRinNgH5ProfLCw2zSJDkQRyCjyBAXUC6916kGREmA51MT28xpYwxbx9PoEx14QMMPtNK4hZQWpYgzev44djZ9URCmb8U2QkpdRaGcBzsHKX29EQw5XFHR2pidkTfypMeUnpdpzmfzrC5q3ZYxJSZQNakabFYBsGRybXvnNtQg2J9QSrz6RbgfZe2Pu2BhzmCw4fXiE7FmsT16RVXPi2nVKUqdc5U8mpGjuK89izCwZmMjyFpSF92DL51vPfE8PptNeBuZCh3ZfUzZTF3Us8DqGs9CAmswduyf48hAsGu6xFNzTt2uhznaHd4KcJdvwN9x6wUboUSRwfJQ8zWuvG3Dhhcpq6EegZ67NZjJ2zhM", cout);		



	//ts.tx_send(" KrjuM9X2UsU3DFa9FbRRZQyZVdpoaiEJLKzf1Ki8aL6uuXmtfSRWN4gUEfeidHdkzYp3dkL9XjEECrLRygd4rn3YG6wtjuVRinNgH5ProfLCw2zSJDkQRyCjyBAXUC6916kGREmA51MT28xpYwxbx9PoEx14QMMPtNK4hZQWpYgzev44djZ9URCmb8U2QkpdRaGcBzsHKX29EQw5XFHR2pidkTfypMeUnpdpzmfzrC5q3ZYxJSZQNakabFYBsGRybXvnNtQg2J9QSrz6RbgfZe2Pu2BhzmCw4fXiE7FmsT16RVXPi2nVKUqdc5U8mpGjuK89izCwZmMjyFpSF92DL51vPfE8PptNeBuZCh3ZfUzZTF3Us8DqGs9CAmswduyf48hAsGu6xFNzTt2uhznaHd4KcJdvwN9x6wUboUSRwfJQ8zWuvG3Dhhcpq6EegZ67NZjJ2zhM",cout);


	
//ts.add_address(crypto::ec::keys::priv_t::from_b58("9DkAG6KFh6uqD3pbH4a8zLug77zbXfA5CMqHqaRRaYPm") , cout);	


//---------------------------

	//crypto::ec::keys::priv_t::from_b58( --> eliptic curve

	api::priv_key("CJ69dXF5E9CTXG4AGCF3hd5ZYVtEtDAWzSQi5F3Ugkcd",cout); 
	
	
	//ts.gen_keys(cout);

	//cout << "--------------------------------------|" << endl;	

	//ts.new_address(cout);


}











