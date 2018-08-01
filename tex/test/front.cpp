#include "front.h"
#include <cex/front.h>
#include <cex/market.h>
#include <cex/markets.h>
#include <cex/engine.h>
#include <cex/order.h>
#include <iostream>
#include <iomanip>

typedef ::front c;

c::front(counter& cnt): B("class front",cnt) {

}
using namespace std;
using namespace curex::cex;

curex::cex::string pwd=L"11111111";


typedef curex::cex::id id;

auto& out=wcout;

void c::match_same_account_two_spots() {
	out << L"Sell A at awo fixed prices and then Buy voth spots at once, Buy A->Sell A, Sell B->Buy B, Buy B->Sell B" << endl;
	out << L"Using same account, commision paid shall be 0" << endl;
	out << L"Wallets must remain at the same state they were at the beginning" << endl;

	engine e("engines_test/engine7.1",wcout);
	auto acid=e.new_account(pwd,out);
	e.deposit(acid,pwd,gbp::id,L"1000",out);
	e.deposit(acid,pwd,eur::id,L"1000",out);
	e.list_account(acid,pwd,out);
	typedef engine::tradeinput tradeinput;
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"100";
		ti.cur=L"EUR";
		ti.rate=L"1.4700";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"100";
		ti.cur=L"EUR";
		ti.rate=L"1.4750";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		e.trade(acid, pwd, input, out);
	}
	e.liquidity(eur::id,gbp::id,out);
	e.list_account(acid,pwd,out);
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"200";
		ti.cur=L"EUR";
		ti.rate=L"1.4700";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		e.trade(acid, pwd, input, out);
	}
	e.liquidity(eur::id,gbp::id,out);
	e.list_account(acid,pwd,out);
	e.list_trades(acid,pwd,out);

	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"1.4700";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"1.4750";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		e.trade(acid, pwd, input, out);
	}
	e.liquidity(eur::id,gbp::id,out);
	e.list_account(acid,pwd,out);
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"200";
		ti.cur=L"GBP";
		ti.rate=L"1.4750";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		e.trade(acid, pwd, input, out);
	}
	e.liquidity(eur::id,gbp::id,out);
	e.list_account(acid,pwd,out);
	e.list_trades(acid,pwd,out);


	// ----

	{ //Buy EUR
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"100";
		ti.cur=L"EUR";
		ti.rate=L"1.4700";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"100";
		ti.cur=L"EUR";
		ti.rate=L"1.4750";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		e.trade(acid, pwd, input, out);
	}
	e.liquidity(eur::id,gbp::id,out);
	e.list_account(acid,pwd,out);
	{ //Sell EUR
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"200";
		ti.cur=L"EUR";
		ti.rate=L"1.4750";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		e.trade(acid, pwd, input, out);
	}
	e.liquidity(eur::id,gbp::id,out);
	e.list_account(acid,pwd,out);
	e.list_trades(acid,pwd,out);

	{ //BUY GBP
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"1.4700";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"1.4750";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		e.trade(acid, pwd, input, out);
	}
	e.liquidity(eur::id,gbp::id,out);
	e.list_account(acid,pwd,out);
	{//Sell GBP
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"200";
		ti.cur=L"GBP";
		ti.rate=L"1.4700";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		e.trade(acid, pwd, input, out);
	}
	e.liquidity(eur::id,gbp::id,out);
	e.list_account(acid,pwd,out);
	e.list_trades(acid,pwd,out);
}


void c::match_same_account_single_spot() {
	out << L"TEST match_same_account_single_spot" << endl;
	engine e("engines_test/engine6",wcout);

	out << L"Sell A at a fixed price and then Buy it, Buy A->Sell A, Sell B->Buy B, Buy B->Sell B" << endl;
	out << L"Using same account, commision paid shall be 0" << endl;
	out << L"Wallets must remain at the same state they were at the beginning" << endl;

	curex::cex::string pwd=L"8888888888";
	auto acid=e.new_account(pwd,out);
	e.deposit(acid,pwd,gbp::id,L"1000",out);
	e.deposit(acid,pwd,eur::id,L"1000",out);
	e.list_account(acid,pwd,out);

	typedef engine::tradeinput tradeinput;
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"100";
		ti.cur=L"EUR";
		ti.rate=L"1.4700";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		e.trade(acid, pwd, input, out);
	}
	e.liquidity(eur::id,gbp::id,out);
	e.list_account(acid,pwd,out);
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"100";
		ti.cur=L"EUR";
		ti.rate=L"1.4700";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		e.trade(acid, pwd, input, out);
	}
	e.liquidity(eur::id,gbp::id,out);
	e.list_account(acid,pwd,out);

	out << "-----------------------------" << endl;
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"100";
		ti.cur=L"EUR";
		ti.rate=L"1.4700";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		e.trade(acid, pwd, input, out);
	}
	e.liquidity(eur::id,gbp::id,out);
	e.list_account(acid,pwd,out);

	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"100";
		ti.cur=L"EUR";
		ti.rate=L"1.4700";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		e.trade(acid, pwd, input, out);
	}
	e.liquidity(eur::id,gbp::id,out);
	e.list_account(acid,pwd,out);

	out << "-----------------------------" << endl;

	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"1.4700";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		e.trade(acid, pwd, input, out);
	}
	e.liquidity(eur::id,gbp::id,out);
	e.list_account(acid,pwd,out);
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"1.4700";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		e.trade(acid, pwd, input, out);
	}
	out << "liquidity" << endl;
	e.liquidity(eur::id,gbp::id,out);
	out << "account: " << endl;
	e.list_account(acid,pwd,out);

	out << "buying 100 gbp" << endl;
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"1.4700";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		e.trade(acid, pwd, input, out);
	}
	out << "liquidity" << endl;
	e.liquidity(eur::id,gbp::id,out);
	out << "account: " << endl;
	e.list_account(acid,pwd,out);
	out << "selling 100 gbp" << endl;
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"1.4700";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		e.trade(acid, pwd, input, out);
	}
	out << "liquidity" << endl;
	e.liquidity(eur::id,gbp::id,out);
	out << "account: " << endl;
	e.list_account(acid,pwd,out);
}

void c::order_buy_and_sell_on_two_fronts_in_a_single_trade() {
	out << L"TEST order_buy_and_sell_on_two_fronts_in_a_single_trade" << endl;
	engine e("engines_test/engine7",out);
	auto acid=e.new_account(pwd,out);
	e.deposit(acid,pwd,gbp::id,L"1000",out);
	e.deposit(acid,pwd,eur::id,L"1000",out);
	e.list_account(acid,pwd,out);
	typedef engine::tradeinput tradeinput;
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"100";
		ti.cur=L"EUR";
		ti.rate=L"1.4600";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"1.4700";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"100";
		ti.cur=L"EUR";
		ti.rate=L"1.4600";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"1.4700";
		ti.rateunits=L"EUR/GBP";
		input.push_back(ti);	
		}
		e.trade(acid, pwd, input, out);
	}
	e.liquidity(eur::id,gbp::id,out);
	e.list_account(acid,pwd,out);
	e.list_orders(acid,pwd,out);

}


void c::test_encode() {
	out << L"TEST test_encode" << endl;
	using curex::cex::encode;

	out << fixed << setprecision(curex::cex::precision);
	out << L"Test encode" << endl;
	{
	efp net=12343326;
	efp comm=315497;
	out << L"before: net " << decode(net) << L" comm " << decode(comm) << endl;
	auto r=encode(decode_dbl(net),decode_dbl(comm),net+comm);
	out << L" after: net " << decode(r.first) << L" comm " << decode(r.second) << endl;
	}

	{
	auto& os=out;
	using curex::cex::string;
	wcout << L"Test encode()" << endl;
	{ string s=L""; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L"0"; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L"19923"; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L".97886"; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L".1234"; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L".123"; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L".12"; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L".1"; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L"."; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L"1"; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L"00."; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L"12."; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L"12.1"; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L"12.12"; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L"12.123"; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L"12.1234"; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L"12.12345"; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L" 12.12345"; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L" 1a.12345"; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L" 1a.12 45"; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L"1a.12 45 "; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L"1 .1245 "; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L".."; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L"..."; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
	{ string s=L"12.1234."; os << curex::cex::encode(s); os << " from '" << s << "'" << endl; }
		/* expected
		0 from ''
		0 from '0'
		199230000 from '19923'
		0 from '.97886'
		1234 from '.1234'
		1230 from '.123'
		1200 from '.12'
		1000 from '.1'
		0 from '.'
		10000 from '1'
		0 from '00.'
		120000 from '12.'
		121000 from '12.1'
		121200 from '12.12'
		121230 from '12.123'
		121234 from '12.1234'
		0 from '12.12345'
		0 from ' 12.12345'
		0 from ' 1a.12345'
		0 from ' 1a.12 45'
		0 from '1a.12 45 '
		0 from '1 .1245 '
		0 from '..'
		0 from '...'
		0 from '12.1234.'
		*/
	{ efp s=12.1234; os << curex::cex::decode(s); os << " from '" << s << "'" << endl; }
		/*expected
		0.0012 from '12'
		*/
	}
}

void c::test_populate_markets() {
	out << "TEST test_populate_markets" << endl;
	engine e("engines_test/engine4",out);
	e.add_sample_liquidity(out);
	e.liquidity(eur::id,gbp::id,out);

}

void c::trade_two_accounts() {
	out << L"TEST trade_two_accounts" << endl;
	out << L"Sum of account positions at the end (included system account) must be equivalent those at the beginning (must match after converting all to e.g. GBP because this test uses a single AUD/GBP rate )" << endl;

	std::string ehome="engines_test/engine8";
	id id1=0,id2=0;
	{
	engine e(ehome,out);

	id1=e.new_account(pwd,out);
	id2=e.new_account(pwd,out);

	e.deposit(id1,pwd,gbp::id,L"1000",out);
	e.deposit(id2,pwd,aud::id,L"1000",out);

	e.list_account(id1,pwd,out);
	e.list_account(id2,pwd,out);
	typedef engine::tradeinput tradeinput;
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id1, pwd, input, out);
	}
	e.liquidity(aud::id,gbp::id,out);
	e.list_account(id1,pwd,out);
	e.list_account(id2,pwd,out);
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id2, pwd, input, out);
	}
	e.liquidity(aud::id,gbp::id,out);
	e.status(id1,pwd,out);
	e.status(id2,pwd,out);
	e.status_sys_account(out);
	}
	test_restart_engine(ehome,id1,id2);
}

void c::trade_by_sellA_sellB_single_spot(bool disk) {
	out << "TEST trade_by_sellA_sellB_single_spot" << endl;
	out << "equivalent to sell+buy, accounts must show correct values" << endl;
	
	std::string ehome="engines_test/engine9";
	if (!disk) ehome="";
	id id1=0,id2=0;
	{
	engine e(ehome,out);

	id1=e.new_account(pwd,out);
	id2=e.new_account(pwd,out);

	e.deposit(id1,pwd,gbp::id,L"1000",out);
	e.deposit(id2,pwd,aud::id,L"1000",out);

	e.status(id1,pwd,out);
	e.status(id2,pwd,out);
	e.status_sys_account(out);

	e.list_account(id1,pwd,out);
	typedef engine::tradeinput tradeinput;
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id1, pwd, input, out);
	}
	e.liquidity(aud::id,gbp::id,out);
	e.list_account(id1,pwd,out);
	e.list_account(id2,pwd,out);
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"200";
		ti.cur=L"AUD";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id2, pwd, input, out);
	}
	e.liquidity(aud::id,gbp::id,out);
	e.status(id1,pwd,out);
	e.status(id2,pwd,out);
	e.status_sys_account(out);
	}
	if (disk) test_restart_engine(ehome,id1,id2);
}

void c::trade_by_sellB_sellA_single_spot() {
	out << "TEST trade_by_sellB_sellA_single_spot" << endl;
	out << "equivalent to sell+buy, accounts must show correct values" << endl;

	std::string ehome="engines_test/engine10";
	id id1=0,id2=0;
	{
	engine e(ehome,out);
	
	id1=e.new_account(pwd,out);
	id2=e.new_account(pwd,out);

	e.deposit(id1,pwd,aud::id,L"1000",out);
	e.deposit(id2,pwd,gbp::id,L"1000",out);

	e.status(id1,pwd,out);
	e.status(id2,pwd,out);
	e.status_sys_account(out);

	e.list_account(id1,pwd,out);
	typedef engine::tradeinput tradeinput;
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"200";
		ti.cur=L"AUD";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id1, pwd, input, out);
	}
	e.liquidity(aud::id,gbp::id,out);
	e.list_account(id1,pwd,out);
	e.list_account(id2,pwd,out);
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id2, pwd, input, out);
	}
	e.liquidity(aud::id,gbp::id,out);
	e.status(id1,pwd,out);
	e.status(id2,pwd,out);
	e.status_sys_account(out);
	}
	test_restart_engine(ehome,id1,id2);

}

void c::trade_by_buyA_buyB_single_spot() {
	out << "TEST trade_by_buyA_buyB_single_spot" << endl;
	out << "equivalent to sell+buy, accounts must show correct values" << endl;

	std::string ehome="engines_test/engine11";
	id id1=0,id2=0;
	{
	engine e(ehome,out);
	
	id1=e.new_account(pwd,out);
	id2=e.new_account(pwd,out);

	e.deposit(id1,pwd,aud::id,L"1000",out);
	e.deposit(id2,pwd,gbp::id,L"1000",out);

	e.status(id1,pwd,out);
	e.status(id2,pwd,out);
	e.status_sys_account(out);

	e.list_account(id1,pwd,out);
	typedef engine::tradeinput tradeinput;
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id1, pwd, input, out);
	}
	e.liquidity(aud::id,gbp::id,out);
	e.list_account(id1,pwd,out);
	e.list_account(id2,pwd,out);
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"200";
		ti.cur=L"AUD";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id2, pwd, input, out);
	}
	e.liquidity(aud::id,gbp::id,out);
	e.status(id1,pwd,out);
	e.status(id2,pwd,out);
	e.status_sys_account(out);
	}
	test_restart_engine(ehome,id1,id2);
}

void c::trade_by_buyB_buyA_single_spot() {
	out << "TEST trade_by_buyB_buyA_single_spot" << endl;
	out << "equivalent to sell+buy, accounts must show correct values" << endl;

	std::string ehome="engines_test/engine12";
	id id1=0,id2=0;
	{
	engine e(ehome,out);
	
	id1=e.new_account(pwd,out);
	id2=e.new_account(pwd,out);

	e.deposit(id1,pwd,gbp::id,L"1000",out);
	e.deposit(id2,pwd,aud::id,L"1000",out);

	e.status(id1,pwd,out);
	e.status(id2,pwd,out);
	e.status_sys_account(out);

	e.list_account(id1,pwd,out);
	typedef engine::tradeinput tradeinput;
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"200";
		ti.cur=L"AUD";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id1, pwd, input, out);
	}
	e.liquidity(aud::id,gbp::id,out);
	e.list_account(id1,pwd,out);
	e.list_account(id2,pwd,out);
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id2, pwd, input, out);
	}
	e.liquidity(eur::id,gbp::id,out);
	e.status(id1,pwd,out);
	e.status(id2,pwd,out);
	e.status_sys_account(out);
	}
	test_restart_engine(ehome,id1,id2);

}

void c::trade_by_sellA_buyA_single_spot() {
	out << "TEST trade_by_sellA_buyA_single_spot" << endl;
	out << "equivalent to sell+buy, accounts must show correct values" << endl;

	std::string ehome="engines_test/engine13";
	id id1=0,id2=0;
	{
	engine e(ehome,out);
	
	id1=e.new_account(pwd,out);
	id2=e.new_account(pwd,out);

	e.deposit(id1,pwd,aud::id,L"1000",out);
	e.deposit(id2,pwd,gbp::id,L"1000",out);

	e.status(id1,pwd,out);
	e.status(id2,pwd,out);
	e.status_sys_account(out);

	e.list_account(id1,pwd,out);
	typedef engine::tradeinput tradeinput;
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"200";
		ti.cur=L"AUD";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id1, pwd, input, out);
	}
	e.liquidity(aud::id,gbp::id,out);
	e.list_account(id1,pwd,out);
	e.list_account(id2,pwd,out);
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"200";
		ti.cur=L"AUD";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id2, pwd, input, out);
	}
	e.liquidity(aud::id,gbp::id,out);
	e.status(id1,pwd,out);
	e.status(id2,pwd,out);
	e.status_sys_account(out);
	}
	test_restart_engine(ehome,id1,id2);
}

void c::trade_by_sellB_buyB_single_spot() {
	out << "TEST trade_by_sellB_buyB_single_spot" << endl;
	out << "equivalent to sell+buy, accounts must show correct values" << endl;

	std::string ehome="engines_test/engine15";
	id id1=0,id2=0;
	{
	engine e(ehome,out);
	
	id1=e.new_account(pwd,out);
	id2=e.new_account(pwd,out);

	e.deposit(id1,pwd,gbp::id,L"1000",out);
	e.deposit(id2,pwd,aud::id,L"1000",out);

	e.status(id1,pwd,out);
	e.status(id2,pwd,out);
	e.status_sys_account(out);

	e.list_account(id1,pwd,out);
	typedef engine::tradeinput tradeinput;
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id1, pwd, input, out);
	}
	e.liquidity(aud::id,gbp::id,out);
	e.list_account(id1,pwd,out);
	e.list_account(id2,pwd,out);
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id2, pwd, input, out);
	}
	e.liquidity(aud::id,gbp::id,out);
	e.status(id1,pwd,out);
	e.status(id2,pwd,out);
	e.status_sys_account(out);
	}
	test_restart_engine(ehome,id1,id2);
}

void c::trade_by_buyA_sellA_single_spot() {
	out << "TEST trade_by_buyA_sellA_single_spot" << endl;
	out << "equivalent to sell+buy, accounts must show correct values" << endl;

	std::string ehome="engines_test/engine14";
	id id1=0,id2=0;
	{
	engine e(ehome,out);
	
	id1=e.new_account(pwd,out);
	id2=e.new_account(pwd,out);

	e.deposit(id1,pwd,gbp::id,L"1000",out);
	e.deposit(id2,pwd,aud::id,L"1000",out);

	e.status(id1,pwd,out);
	e.status(id2,pwd,out);
	e.status_sys_account(out);

	e.list_account(id1,pwd,out);
	typedef engine::tradeinput tradeinput;
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"200";
		ti.cur=L"AUD";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id1, pwd, input, out);
	}
	e.liquidity(aud::id,gbp::id,out);
	e.list_account(id1,pwd,out);
	e.list_account(id2,pwd,out);
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"200";
		ti.cur=L"AUD";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id2, pwd, input, out);
	}
	e.liquidity(aud::id,gbp::id,out);
	e.status(id1,pwd,out);
	e.status(id2,pwd,out);
	e.status_sys_account(out);
	}
	test_restart_engine(ehome,id1,id2);
}

void c::trade_by_buyB_sellB_single_spot() {
	out << "TEST trade_by_buyB_sellB_single_spot" << endl;
	out << "equivalent to sell+buy, accounts must show correct values" << endl;

	std::string ehome="engines_test/engine16";
	id id1=0,id2=0;
	{

	engine e(ehome,out);

	id1=e.new_account(pwd,out);
	id2=e.new_account(pwd,out);

	e.deposit(id1,pwd,aud::id,L"1000",out);
	e.deposit(id2,pwd,gbp::id,L"1000",out);

	e.status(id1,pwd,out);
	e.status(id2,pwd,out);
	e.status_sys_account(out);

	e.list_account(id1,pwd,out);
	typedef engine::tradeinput tradeinput;
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id1, pwd, input, out);
	}
	e.liquidity(aud::id,gbp::id,out);
	e.list_account(id1,pwd,out);
	e.list_account(id2,pwd,out);
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"100";
		ti.cur=L"GBP";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id2, pwd, input, out);
	}
	e.liquidity(aud::id,gbp::id,out);
	e.status(id1,pwd,out);
	e.status(id2,pwd,out);
	e.status_sys_account(out);
	}
	test_restart_engine(ehome,id1,id2);
}

	curex::cex::string strcast(const std::string& o) {
		curex::cex::string ans;
		ans.assign(o.begin(), o.end());
		return ans;
	}

void c::test_restart_engine(std::string home,id id1,id id2) {
	out << L"TEST restart engine " << strcast(home) << endl;
	engine e(home,out);
	e.status(id1,pwd,out);
	e.status(id2,pwd,out);
	e.status_sys_account(out);
}

#include <thread>
struct concur_test {
	typedef engine::tradeinput tradeinput;
	concur_test(): e("engines_test/concurrency",out) {
	}
	std::thread spawn() {
	      return std::thread( [this] { this->main(); } );
	}

	void main() {
//		curex::cex out(0);		
		id id1=e.new_account(pwd,out);
		e.delete_wallet(id1,pwd,eur::id,out);
		e.new_wallet(id1,pwd,aud::id,out);
		bool flag=false;
		for (int i=1; i<5; ++i) {
		flag=!flag;
		e.deposit(id1,pwd,aud::id,L"1000",out);
		e.deposit(id1,pwd,gbp::id,L"1000",out);
		e.status(id1,pwd,out);
		e.status_sys_account(out);
		e.list_account(id1,pwd,out);
		{
			std::vector<tradeinput> input;
			{
			tradeinput ti;
			ti.code=L"S";
			ti.amount=L"100";
			ti.cur=L"GBP";
			ti.rate=L"2.0000";
			ti.rateunits=L"AUD/GBP";
			input.push_back(ti);	
			}
			e.trade(id1, pwd, input, out);
		}
		{
			std::vector<tradeinput> input;
			{
			tradeinput ti;
			ti.code=L"S";
			ti.amount=L"100";
			ti.cur=L"AUD";
			ti.rate=L"2.0000";
			ti.rateunits=L"AUD/GBP";
			input.push_back(ti);	
			}
			e.trade(id1, pwd, input, out);
		}
		{
			std::vector<tradeinput> input;
			{
			tradeinput ti;
			ti.code=L"B";
			ti.amount=L"100";
			ti.cur=L"AUD";
			ti.rate=L"2.0000";
			ti.rateunits=L"AUD/GBP";
			input.push_back(ti);	
			}
			e.trade(id1, pwd, input, out);
		}
		{
			std::vector<tradeinput> input;
			{
			tradeinput ti;
			ti.code=L"B";
			ti.amount=L"100";
			ti.cur=L"GBP";
			ti.rate=L"2.0000";
			ti.rateunits=L"AUD/GBP";
			input.push_back(ti);	
			}
			e.trade(id1, pwd, input, out);
		}
		e.liquidity(aud::id,gbp::id,out);
		e.withdraw(id1,pwd,gbp::id,L"10",out);
		e.valid_rates(gbp::id,aud::id,out);
		e.valid_rates_simple(gbp::id,aud::id,out);
		e.list_markets(out);
		e.list_currencies(out);
		{
			std::vector<tradeinput> input;
			{
			tradeinput ti;
			ti.code=L"B";
			ti.amount=L"100";
			ti.cur=L"GBP";
			ti.rate=L"2.0000";
			ti.rateunits=L"AUD/GBP";
			input.push_back(ti);	
			}
			e.trade(id1, pwd, input, out);
		}
		e.cancel_all_orders(id1,pwd,out);
		e.list_trades(id1,pwd,out);
		{
			std::vector<tradeinput> input;
			{
			tradeinput ti;
			ti.code=L"B";
			ti.amount=L"100";
			ti.cur=L"GBP";
			ti.rate=L"2.0000";
			ti.rateunits=L"AUD/GBP";
			input.push_back(ti);	
			}
			e.trade(id1, pwd, input, out);
		}
		auto orders=e.get_orders(id1, pwd);
		for (auto& o:orders) {
			if (o.key==L'U') {
				if (flag) 
				e.update_order(id1,pwd,o._id,L"150",out);
				else
				e.update_order(id1,pwd,o._id,L"10",out);
				break;
			}
		}
		}		
		
	}

	engine e;

};

void c::tradelow() {
	out << "TEST tradelow" << endl;

	std::string ehome="engines_test/tradelow";
	id id1=0,id2=0;
	{

	engine e(ehome,out);

	id1=e.new_account(pwd,out);

	e.deposit(id1,pwd,aud::id,L"1000",out);
	e.deposit(id1,pwd,gbp::id,L"1000",out);

	e.status(id1,pwd,out);
	e.status_sys_account(out);

	out << L"1.-ponemos 100 en la cola de baja prioridad" << endl;
	typedef engine::tradeinput tradeinput;
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"100";
		ti.cur=L"AUD";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade_low(id1, pwd, input, out);
	}
	out << L"1.5.-el id de la linea U es de prioridad baja " << endl;
	out << L"2.-ponemos 100 en la cola de normal" << endl;
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"S";
		ti.amount=L"100";
		ti.cur=L"AUD";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id1, pwd, input, out);
	}
	out << L"2.5.-el nuevo id de la linea U es de prioridad normal " << endl;
	e.liquidity(aud::id,gbp::id,out);
	out << L"3.-compramos 50 " << endl;
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"50";
		ti.cur=L"AUD";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id1, pwd, input, out);
	}
	out << L"4.-debe quedar unmatched 100 del id 1 y 50 del id 2 (mas prioridad aunque haya entrado despues)" << endl;
	e.liquidity(aud::id,gbp::id,out);
	out << L"5.-compramos 100 mas" << endl;
	{
		std::vector<tradeinput> input;
		{
		tradeinput ti;
		ti.code=L"B";
		ti.amount=L"100";
		ti.cur=L"AUD";
		ti.rate=L"2.0000";
		ti.rateunits=L"AUD/GBP";
		input.push_back(ti);	
		}
		e.trade(id1, pwd, input, out);
	}
	out << L"6.-debe quedar unmatched 50 del id 1" << endl;
	e.liquidity(aud::id,gbp::id,out);
	e.list_orders(id1,pwd,out);
	}
}

void c::test_concurrency() {
	vector<thread> v;
	concur_test t;

	for (int i=0; i<2; ++i)
		v.emplace_back(t.spawn());

	for (auto& t: v)
		t.join();

}


void c::run() {
	B::run();

	test_encode();
	test_populate_markets();
	match_same_account_single_spot();
	match_same_account_two_spots();
	order_buy_and_sell_on_two_fronts_in_a_single_trade();
	trade_two_accounts();
	trade_by_sellA_sellB_single_spot(true);
	trade_by_sellA_sellB_single_spot(false);
	trade_by_sellB_sellA_single_spot();
	trade_by_buyA_buyB_single_spot();
	trade_by_buyB_buyA_single_spot();
	trade_by_sellA_buyA_single_spot();
	trade_by_buyA_sellA_single_spot();
	trade_by_sellB_buyB_single_spot();
	trade_by_buyB_sellB_single_spot();
	tradelow();

	//test_update();
//	test_concurrency();


}	

	/*
	{
		curex::cex::front::rates rs;
		curex::cex::front f(0,false);
		{
		rs.emplace(encode(L"1.4500"));
		rs.emplace(encode(L"1.4515"));
		rs.emplace(encode(L"1.4520"));
		rs.emplace(encode(L"1.4525"));
		f.set_rates(rs);
		}

		f.new_order(encode(L"1.4500"),encode(L"250"),true,false,wcout);
		f.new_order(encode(L"1.4515"),encode(L"250"),true,false,wcout);

		f.new_order(encode(L"1.4520"),encode(L"100"),true,false,wcout);
		const order* id321=f.new_order(encode(L"1.4520"),encode(L"321"),true,false,wcout);
		f.new_order(encode(L"1.4520"),encode(L"100"),true,false,wcout);

		const order* id2=f.new_order(encode(L"1.4525"),encode(L"416.8744"),true,false,wcout);
		f.new_order(encode(L"1.4525"),encode(L"250"),true,false,wcout);

		f.dump(L"",wcout);

		wcout << L"cancelling order " << id321->_id << endl;
		f.cancel_order(id321->_id,wcout);
		f.dump(L"",wcout);

		{
		wcout << L"updating order " << id2->_id << L" reducing to 411.1092" << endl;
		const order*  n=f.update_order(id2->_id,encode(L"411.1092"),wcout);
		wcout << L"returned id " << (n==0?0:n->_id) << endl;
		f.dump(L"",wcout);
		}
		{
		wcout << L"updating order " << id2->_id << L" increasing to 511.0001" << endl;
		const order*  n=f.update_order(id2->_id,encode(L"511.0001"),wcout);
		wcout << L"returned id " << (n==0?0:n->_id) << endl;
		f.dump(L"",wcout);
		}
		{
		wcout << L"updating order " << id2->_id << L" reducing to 0" << endl;
		const order*  n=f.update_order(id2->_id,encode(L"0"),wcout);
		wcout << L"returned id " << (n==0?0:n->_id) << endl;
		f.dump(L"",wcout);
		}


		{
		wcout << L"cancelling order 1" << endl;
		const order*  n=f.update_order(1,encode(L"0"),wcout);
		f.dump(L"",wcout);
		}

		{
		wcout << L"cancelling order 2,3,5,7,8" << endl;
		f.cancel_order(2,wcout);
		f.cancel_order(3,wcout);
		f.cancel_order(5,wcout);
		f.cancel_order(7,wcout);
		f.cancel_order(8,wcout);
		f.dump(L"",wcout);
		}
	}
	{
		//curex::cex::front<curex::cex::waiting_queue, weaker_currency_comp> F;
		curex::cex::front::rates rs;
		curex::cex::front F(0,true);
		{
		rs.emplace(encode(L"1.4500"));
		rs.emplace(encode(L"1.4515"));
		rs.emplace(encode(L"1.4520"));
		rs.emplace(encode(L"1.4525"));
		F.set_rates(rs);
		}
		{
		wcout << endl << L"populating weaker front" << endl;
		F.new_order(encode(L"1.4500"),encode(L"250"),true,false,wcout);
		F.new_order(encode(L"1.4515"),encode(L"250"),true,false,wcout);

		F.new_order(encode(L"1.4520"),encode(L"100"),true,false,wcout);
		const order* id321=F.new_order(encode(L"1.4520"),encode(L"321"),true,false,wcout);
		F.new_order(encode(L"1.4520"),encode(L"100"),true,false,wcout);

		const order* id2=F.new_order(encode(L"1.4525"),encode(L"416.8744"),true,false,wcout);
		F.new_order(encode(L"1.4525"),encode(L"250"),true,false,wcout);
		F.dump(L"",wcout);
		}
		curex::cex::wallet w;
		w.deposit(100000);
		{
		wcout << endl << L"matching 1.4530 25 se espera ningun match" << endl;
		matches ms=F.match(w,encode(L"1.4530"),encode(L"25"),eur::id,gbp::id,false,wcout);
		ms.dump(L"",wcout);
		F.dump(L"",wcout);
		}

		{
		wcout << endl << L"matching 1.4522 25 se espera 1 match" << endl;
		matches ms=F.match(w,encode(L"1.4522"),encode(L"25"),eur::id,gbp::id,false,wcout);
		ms.dump(L"",wcout);
		F.dump(L"",wcout);
		}
		{
		wcout << endl << L"matching 1.4522 400 se espera 2 match" << endl;
		matches ms=F.match(w,encode(L"1.4522"),encode(L"400"),eur::id,gbp::id,false,wcout);
		ms.dump(L"",wcout);
		F.dump(L"",wcout);
		}
		{
		wcout << endl << L"matching 1.1 80 se espera 2 match de 2 rates diferentes" << endl;
		matches ms=F.match(w,encode(L"1.1"),encode(L"80"),eur::id,gbp::id,false,wcout);
		ms.dump(L"",wcout);
		F.dump(L"",wcout);
		}
		{
		wcout << endl << L"matching 1.1 10000 se espera machearlo todo" << endl;
		matches ms=F.match(w,encode(L"1.1"),encode(L"10000"),eur::id,gbp::id,false,wcout);
		ms.dump(L"",wcout);
		F.dump(L"",wcout);
		}
	}
*/

/*
	{
	
	engine e("engines_test/engine1",wcout);
	e.set_ruler(eur::id,gbp::id,1.00,2.00);
	e.set_ref_rate(eur::id,gbp::id,1.5);

	e.valid_rates(eur::id,gbp::id,wcout);
	id acid=e.new_account(pwd,wcout);
	e.deposit(acid,pwd,eur::id,L"1000",wcout);
	e.deposit(acid,pwd,gbp::id,L"1000",wcout);
	e.liquidity(eur::id,gbp::id,1,wcout);

	id o=e.trade(acid,pwd,eur::id,L"500",L"2.0",gbp::id,wcout);
	e.list_orders(acid,pwd,wcout);
	auto ro=e.update_order(acid,pwd,o,L"300",wcout);
	wcout << "returned order #" << ro << endl;
	e.list_orders(acid,pwd,wcout);
	wcout << "update order+ " << o << endl;
	auto r2=e.update_order(acid,pwd,o,L"600",wcout);
	wcout << "returned order " << r2 << endl;
	wcout << "listing orders " << endl;
	e.list_orders(acid,pwd,wcout);
	
	id acid2=e.new_account(pwd,wcout);
	e.deposit(acid2,pwd,gbp::id,L"1000",wcout);
	e.trade(acid2,pwd,gbp::id,L"600",L"2.00",eur::id,wcout);


	}

	struct client: public engine {
		id _id;
		client(): engine("engines_test/engine2",wcout) {
			_id=new_account(pwd,wcout);
			deposit(_id,pwd,eur::id,L"1000",wcout);
			valid_rates(eur::id,gbp::id,wcout);
		}

		void lay() {
	//id c::trade(const id& acid, const string&pwd, const id& cid, const double& amount, double limit_rate, int target_cid, ostream&err)

			id o=trade(_id,pwd,eur::id,L"500",L"2.0",gbp::id,wcout);
			//dump(L"",wcout);
			update_order(_id,pwd,o,L"450",wcout);
			cancel_order(_id,pwd,o,wcout);
		}
		
	};


	{

cout << "-------------------------------------------------p22" << endl;
	client c;
	c.lay();
	//c.dump(L"",wcout);
cout << "-----------------------------------------------/-p22" << endl;
	}

	{
	market::key k1(1,3);
	market::key k2(3,1);
	if (k1==k2) cout << "keys iguales" << endl;
	cout << "hash key1 " << market::key_hasher()(k1) << endl;
	cout << "hash key2 " << market::key_hasher()(k2) << endl;
	}

	{

	engine e("engines_test/engine3",wcout);
	auto& out=wcout;
	cout << "================================================ " << endl;

		curex::cex::string pwd=L"88888888";
		out << "account: ";
		auto acid=e.new_account(pwd,out);
		e.list_account(acid,pwd,out);
		out << "depositing" << endl;
		e.deposit(acid,pwd,gbp::id,L"2100",out);
		e.deposit(acid,pwd,eur::id,L"2100",out);
		e.deposit(acid,pwd,1,L"13200",out);
		e.deposit(acid,pwd,2,L"15400.123456",out);
		e.deposit(acid,pwd,3,L"10430.500",out);
		e.deposit(acid,pwd,4,L"10000",out);
		out << "account: " << endl;
		curex::cex::ostringstream depositos;
		out << depositos.str() << endl;
		e.list_account(acid,pwd,depositos);
		out << "markets: " << endl;
		e.list_markets(out);
		out << "valid rates: " << endl;
		e.valid_rates(eur::id,gbp::id,out);
		out << "placing order 500 eur for gbp at 1.5" << endl;
		e.trade(acid,pwd,eur::id,L"500",L"1.5",gbp::id,out);
		out << "listing orders " << endl;
		e.list_orders(acid,pwd,out);
		out << "liquidity EUR GBP" << endl;
		e.liquidity(eur::id,gbp::id,out);
		out << "account: " << endl;
		e.list_account(acid,pwd,out);
		out << "placing order 350.667 gbp for eur at " << 1.5 << endl;
		curex::cex::id o=e.trade(acid,pwd,gbp::id,L"350.667",L"1.5",eur::id,out);
		out << "listing orders " << endl;
		e.list_orders(acid,pwd,out);
		out << "account: " << endl;
		e.list_account(acid,pwd,out);
		out << "update order " << o << " +" << endl;
		curex::cex::id o2=e.update_order(acid,pwd,o,L"500",out);
		out << "listing orders " << endl;
		e.list_orders(acid,pwd,out);
		out << "update order " << o2 << " -" << endl;
		e.update_order(acid,pwd,o2,L"71.5468",out);
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
		e.trade(acid,pwd,gbp::id,L"600",L"0.85",eur::id,out);
		out << "A2" << endl;
		e.trade(acid,pwd,gbp::id,L"500",L"0.90",eur::id,out);
		out << "A3" << endl;
		e.trade(acid,pwd,gbp::id,L"400",L"0.95",eur::id,out);
		out << "A4" << endl;
		e.trade(acid,pwd,gbp::id,L"300",L"1.00",eur::id,out);
		out << "A5" << endl;
		e.trade(acid,pwd,gbp::id,L"200",L"1.05",eur::id,out);
		out << "A6" << endl;
		e.trade(acid,pwd,gbp::id,L"100",L"1.10",eur::id,out);
		out << "liquidity" << endl;
		e.liquidity(gbp::id,eur::id,out);



		out << "B1" << endl;
		e.trade(acid,pwd,eur::id,L"600",L"1.4",gbp::id,out);
		out << "B2" << endl;
		e.trade(acid,pwd,eur::id,L"500",L"1.35",gbp::id,out);
		out << "B3" << endl;
		e.trade(acid,pwd,eur::id,L"400",L"1.3",gbp::id,out);
		out << "B4" << endl;
		e.trade(acid,pwd,eur::id,L"300",L"1.25",gbp::id,out);
		out << "B5" << endl;
		e.trade(acid,pwd,eur::id,L"200",L"1.2",gbp::id,out);
		out << "B6" << endl;
		e.trade(acid,pwd,eur::id,L"100",L"1.15",gbp::id,out);

		out << "liquidity" << endl;
		e.liquidity(gbp::id,eur::id,out);

		out << "account: " << endl;
		e.list_account(acid,pwd,out);

		out << "matching all GBP - 2500 @ 0.9" << endl;
		e.trade(acid,pwd,eur::id,L"2500",L"0.9",gbp::id,out);

		out << "liquidity" << endl;
		e.liquidity(gbp::id,eur::id,out);

		out << "matching eur up to 1.30 - 4000 @ 1.30" << endl;
		e.trade(acid,pwd,gbp::id,L"4000",L"1.3",eur::id,out);

		out << "liquidity" << endl;
		e.liquidity(gbp::id,eur::id,out);

		out << "listing orders " << endl;
		e.list_orders(acid,pwd,out);

		out << "cancelling all orders" << endl;
		e.cancel_all_orders(acid,pwd,out);

		out << "account: " << endl;
		e.list_account(acid,pwd,out);

		out << "account original: " << endl;
		out << depositos.str() << endl;

	}
*/
/*
	{
		wcout << "loading sample liquidity" << endl;
	engine e("engines_test/sample_liquidity",wcout);
		wcout << "liquidity" << endl;
		e.liquidity(gbp::id,eur::id,wcout);
	}
*/
/*
	{
	
	auto& out=wcout;
	engine e("engines_test/engine5",out);
	e.add_sample_liquidity(out);
	out << "================================================ " << endl;

		curex::cex::string pwd=L"8888888888";
		out << "account: ";
		auto acid=e.new_account(pwd,out);
		e.list_account(acid,pwd,out);
		out << "depositing" << endl;
		e.deposit(acid,pwd,gbp::id,L"2100",out);
		e.deposit(acid,pwd,eur::id,L"2100",out);
		e.deposit(acid,pwd,1,L"13200",out);
		e.deposit(acid,pwd,2,L"15400.123456",out);
		e.deposit(acid,pwd,3,L"10430.500",out);
		e.deposit(acid,pwd,4,L"10000",out);
		out << "account: " << endl;
		curex::cex::string frontrate;
		{
		curex::cex::ostringstream os;
		e.liquidity(eur::id,gbp::id,1,os);
		curex::cex::istringstream is(os.str());
		is >> frontrate;
		}
		out << "front rate " << frontrate << endl;
		e.liquidity(eur::id,gbp::id,1,out);
		out << "selling 100 eur @ " << frontrate << endl;
		e.trade(acid,pwd,eur::id,L"100",frontrate,gbp::id,out);
		out << "liquidity" << endl;
		e.liquidity(eur::id,gbp::id,1,out);
		out << "buying 10 gbp @ " << frontrate << endl;
		e.trade(acid,pwd,gbp::id,L"10",frontrate,eur::id,out);
		out << "liquidity" << endl;
		e.liquidity(eur::id,gbp::id,1,out);
	}
*/

