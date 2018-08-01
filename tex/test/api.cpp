#include "api.h"
#include <cexr/client.h>
#include <iostream>

typedef ::api c;

c::api(counter& cnt): B("class client",cnt) {

}
using namespace std;
using namespace curex;



typedef client::id id;
void c::run() {
	B::run();

client c;

string pwd="88888888";
client cli;
cout << "account: ";
auto acid=cli.new_account(pwd);
/*
e.list_account(acid,pwd,out);
out << "depositing" << endl;
e.deposit(acid,pwd,gbp::id,2100,out);
e.deposit(acid,pwd,eur::id,2100,out);
e.deposit(acid,pwd,1,13200,out);
e.deposit(acid,pwd,2,15400.123456,out);
e.deposit(acid,pwd,3,10430.500,out);
e.deposit(acid,pwd,4,10000,out);
out << "account: " << endl;
curex::cex::ostringstream depositos;
out << depositos.str() << endl;
e.list_account(acid,pwd,depositos);
out << "markets: " << endl;
e.list_markets(out);
out << "valid rates: " << endl;
e.valid_rates(eur::id,gbp::id,out);
out << "placing order 500 eur for gbp at 1.5" << endl;
e.trade(acid,pwd,eur::id,500,1.5,gbp::id,out);
out << "listing orders " << endl;
e.list_orders(acid,pwd,out);
out << "account: " << endl;
e.list_account(acid,pwd,out);
out << "placing order 350.667 gbp for eur at " << 0.6666 << endl;
curex::cex::id o=e.trade(acid,pwd,gbp::id,350.667,0.6666,eur::id,out);
out << "listing orders " << endl;
e.list_orders(acid,pwd,out);
out << "account: " << endl;
e.list_account(acid,pwd,out);
out << "update order " << o << " +" << endl;
curex::cex::id o2=e.update_order(acid,pwd,o,500,out);
out << "listing orders " << endl;
e.list_orders(acid,pwd,out);
out << "update order " << o2 << " -" << endl;
e.update_order(acid,pwd,o2,71.5468,out);
out << "listing orders " << endl;
e.list_orders(acid,pwd,out);
out << "cancelling order: " << endl;
e.cancel_order(acid,pwd,o,out);
out << "listing orders " << endl;
e.list_orders(acid,pwd,out);
out << "cancelling all orders" << endl;
e.cancel_all_orders(acid,pwd,out);
out << "listing orders " << endl;
e.list_orders(acid,pwd,out);
out << "account: " << endl;
e.list_account(acid,pwd,out);
out << "--------------AAA-------------------------------------------------"<< endl;

out << "populating market" << endl;
out << "A1" << endl;
e.trade(acid,pwd,gbp::id,100,1/1.5,eur::id,out);
out << "A2" << endl;
e.trade(acid,pwd,gbp::id,200,1/1.55,eur::id,out);
out << "A3" << endl;
e.trade(acid,pwd,gbp::id,300,1/1.6,eur::id,out);
out << "A4" << endl;
e.trade(acid,pwd,gbp::id,400,1/1.65,eur::id,out);
out << "A5" << endl;
e.trade(acid,pwd,gbp::id,500,1/1.7,eur::id,out);
out << "A6" << endl;
e.trade(acid,pwd,gbp::id,600,1/1.75,eur::id,out);

out << "liquidity" << endl;
e.liquidity(gbp::id,eur::id,out);

out << "B1" << endl;
e.trade(acid,pwd,eur::id,100,1.4,gbp::id,out);
out << "B2" << endl;
e.trade(acid,pwd,eur::id,200,1.35,gbp::id,out);
out << "B3" << endl;
e.trade(acid,pwd,eur::id,300,1.3,gbp::id,out);
out << "B4" << endl;
e.trade(acid,pwd,eur::id,400,1.25,gbp::id,out);
out << "B5" << endl;
e.trade(acid,pwd,eur::id,500,1.2,gbp::id,out);
out << "B6" << endl;
e.trade(acid,pwd,eur::id,600,1.15,gbp::id,out);

out << "liquidity" << endl;
e.liquidity(gbp::id,eur::id,out);

out << "account: " << endl;
e.list_account(acid,pwd,out);

out << "matching all GBP - 2500 @ 0.8333" << endl;
e.trade(acid,pwd,gbp::id,2500,0.8333,eur::id,out);

out << "liquidity" << endl;
e.liquidity(gbp::id,eur::id,out);

out << "matching eur up to 1.70 - 4000 @ 1.70" << endl;
e.trade(acid,pwd,eur::id,4000,1.7,gbp::id,out);

out << "liquidity" << endl;
e.liquidity(gbp::id,eur::id,out);

out << "cancelling all orders" << endl;
e.cancel_all_orders(acid,pwd,out);

out << "account: " << endl;
e.list_account(acid,pwd,out);

out << "account original: " << endl;
out << depositos.str() << endl;
*/
}	
