#include <boost/date_time/posix_time/posix_time.hpp>
#include <iostream>
#include <thread>
#include <fstream>
#include <fastcgi++/request.hpp>
#include <fastcgi++/manager.hpp>
#include <cex/engine.h>
#include "uri_decode.h"

using namespace std;
using namespace Fastcgipp;
using namespace curex::cex;
typedef wchar_t  chartype;
//typedef char  chartype;
int count_reqs{0};

engine* e{0};
class cexapi: public Request<chartype> {
	typedef basic_string<chartype> string;
	typedef basic_istringstream<chartype> istringstream;
   bool response() {
	++count_reqs;
//out << environment().requestUri << endl;
//out << "count requests " << count_reqs << " " << this << " " << this_thread::get_id()  << endl;
/*
	if (environment().requestUri.size()>100) {
		out << "request is too long to be legitimated. Don't be nasty, you could be banned soon." << endl;
		return true;
	}
*/
	out << fixed << setprecision(curex::cex::precision);

	static const curex::cex::string url=L"https://trocavaluta.com/api";

	auto uri=uri_decode(environment().requestUri);
	istringstream is(uri);
	string command;
	is >> command;

	if (command!=L"/api") {
		out << "Content-Type: text/plain; charset=utf-8" << endl << endl;
	}
	if (command==L"/api/?trade") {
		//ticket_4 S 101 USD 1.4750 AUD/USD ticket_3 S 120 USD 1.4750 AUD/USD ticket_2 B 654.8977 AUD 1.4750 AUD/USD ticket_1 B 33 AUD 1.4750 AUD/USD
		curex::cex::id acid=0;
		is >> acid;
		string password;
		is >> password;
		std::vector<engine::tradeinput> input;
		while(!is.eof()) {
			engine::tradeinput ti;
			is >> ti.id_of_client;
			is >> ti.code;
			is >> ti.amount;
			is >> ti.cur;
			is >> ti.rate;
			is >> ti.rateunits;
			input.emplace_back(move(ti));
		}
		e->trade(acid,password,move(input),out);
	}
	else if (command==L"/api/?update") {
		curex::cex::id acid=0;
		is >> acid;
		string password;
		is >> password;
		std::vector<engine::updateinput> input;
		while(!is.eof()) {
			engine::updateinput ti;
			is >> ti.order_id;
			is >> ti.amount;
			is >> ti.rate;
			input.emplace_back(move(ti));
		}
		e->update(acid,password,move(input),out);
	}
	else if (command==L"/api/?update_order") {
		curex::cex::id acid=0;
		is >> acid;
		string password;
		is >> password;
		curex::cex::id order_id=0;
		is >> order_id;
		string amount;
		is >> amount;
		e->update_order(acid,password,order_id,amount,out);
	}
	else if (command==L"/api/?list_orders") {
		curex::cex::id acid=0;
		is >> acid;
		string password;
		is >> password;
		e->list_orders(acid,password,out);
	}
	else if (command==L"/api/?list_trades") {
		curex::cex::id acid=0;
		is >> acid;
		string password;
		is >> password;
		e->list_trades(acid,password,out);
	}
	else if (command==L"/api/?status") {
		curex::cex::id acid=0;
		is >> acid;
		string password;
		is >> password;
		e->status(acid,password,out);
	}
	else if (command==L"/api/?new_account") {
		string password;
		is >> password;
		auto acid=e->new_account(password,out);
		if (acid!=0)
			out << acid << endl;
		
	}
	else if (command==L"/api/?liquidity") {
		curex::cex::id cid1=0;
		is >> cid1;
		curex::cex::id cid2=0;
		is >> cid2;
		unsigned int depth=-1; //maximum depth
		is >> depth;
		e->liquidity(cid1,cid2,depth,out);
	}
	else if (command==L"/api/?cancel_all_orders") {
		curex::cex::id acid=0;
		is >> acid;
		string password;
		is >> password;
		e->cancel_all_orders(acid,password,out);
	}
	else if (command==L"/api/?list_currencies") {
		e->list_currencies(out);
	}
	else if (command==L"/api/?list_account") {
		curex::cex::id acid=0;
		is >> acid;
		string password;
		is >> password;
		e->list_account(acid,password,out);
	}
	else if (command==L"/api/?list_markets") {
		e->list_markets(out);
	}
	else if (command==L"/api/?valid_rates_simple") {
		curex::cex::id cid1=0;
		is >> cid1;
		curex::cex::id cid2=0;
		is >> cid2;
		e->valid_rates_simple(cid1,cid2,out);
	}
	else if (command==L"/api/?valid_rates") {
		curex::cex::id cid1=0;
		is >> cid1;
		curex::cex::id cid2=0;
		is >> cid2;
		e->valid_rates(cid1,cid2,out);
	}
	else if (command==L"/api/?deposit") {
		curex::cex::id acid=0;
		is >> acid;
		string password;
		is >> password;
		curex::cex::id cid=0;
		is >> cid;
		string amount;
		is >> amount;
		e->deposit(acid,password,cid,amount,out);
	}
	else if (command==L"/api/?withdraw") {
		curex::cex::id acid=0;
		is >> acid;
		string password;
		is >> password;
		curex::cex::id cid=0;
		is >> cid;
		string amount;
		is >> amount;
		e->withdraw(acid,password,cid,amount,out);
	}
	else if (command==L"/api/?new_wallet") {
		curex::cex::id acid=0;
		is >> acid;
		string password;
		is >> password;
		curex::cex::id cid=0;
		is >> cid;
		e->new_wallet(acid,password,cid,out);
	}
	else if (command==L"/api/?delete_wallet") {
		curex::cex::id acid=0;
		is >> acid;
		string password;
		is >> password;
		curex::cex::id cid=0;
		is >> cid;
		e->delete_wallet(acid,password,cid,out);
	}
	else {
		out << "Content-Type: text/html; charset=utf-8" << endl << endl;
		out << "<pre>" << endl;
		out << "TROCAVALUTA - Low cost currency exchange for all." << endl;
		out << "API functions" << endl;
		out << endl;
		out << url << "/?<b>new_account</b> <password>" << endl;
		out << "    Creates a new account and return its id" << endl;
		out << "    <password> cannot contain spaces and must be at least 8 characters long." << endl;
		out << "    The password given must be remembered for sucesive api calls." << endl;
		out << endl;
		out << url << "/?<b>new_wallet</b> <account_id> <password> <currency id>" << endl;
		out << "    Creates a new wallet to store money in the specified currency" << endl;
		out << endl;
		out << url << "/?<b>delete_wallet</b> <account_id> <password> <currency id>" << endl;
		out << "    Deletes an existing wallet only if it is empty." << endl;
		out << endl;
		out << url << "/?<b>list_currencies</b>" << endl;
		out << "    lists available currencies along with their ids" << endl;
		out << endl;
		out << url << "/?<b>deposit</b> <account_id> <password> <currency_id> <amount>" << endl;
		out << "    add funds to your account." << endl;
		out << "    example:" << endl;
		out << "    /?deposit 19290 x291058 1 155.26" << endl;
		out << "    will fund the account with id 19290 with 155.26 EUR, provided the password given is correct." << endl;
		out << endl;
		out << url << "/?<b>list_account</b> <acid> <password>" << endl;
		out << "    retrieve current available funds." << endl;
		out << "    Columns in the response are "; curex::cex::engine::account_columns(out);
		out << endl;
		out << url << "/?<b>list_markets</b>" << endl;
		out << "    Retrieve the list of available markets." << endl;
		out << "    Columns in the response are: "; curex::cex::engine::markets_columns(out);
		out << endl;
		out << url << "/?<b>valid_rates</b> <currrency id> <currrency id>" << endl;
		out << "    Retrieve the list of allowed rates for the specified currencies." << endl;
		out << "    Columns in the response are: "; curex::cex::engine::valid_rates_columns(out);
		out << endl;
		out << url << "/?<b>valid_rates_simple</b> <currrency id> <currrency id>" << endl;
		out << "    Provide the same fundamental information as the function 'valid_rates' but less verbose." << endl;
		out << "    Columns in the response are: "; curex::cex::engine::valid_rates_simple_columns(out);
		out << endl;
		out << url << "/?<b>status</b> <account_id> <password>" << endl;
		out << "    Receive the combined response of list_orders, list_trades and list_account." << endl;
		out << "    Columns in the response are: " << endl;
		out << "       orders: "; curex::cex::engine::unmatches_columns(out);
		out << "      matches: "; curex::cex::engine::trades_columns(out);
		out << "      wallets: "; curex::cex::engine::account_columns(out);
		out << endl;
		out << url << "/?<b>trade</b> <account_id> <password> <currency id> <amount> <rate> <target currency id>" << endl;
		out << "    Place the desired amount to trade for the specified currency at the specified rate." << endl;
		out << "    Response is same as status." << endl;
		out << endl;
//ticket_4 S 101 USD 1.4750 AUD/USD ticket_3 S 120 USD 1.4750 AUD/USD ticket_2 B 654.8977 AUD 1.4750 AUD/USD ticket_1 B 33 AUD 1.4750 AUD/USD
		out << url << "/?<b>trade</b> <account_id> <password> {<yourid> <B|S> <amount> <currency> <rate> <rate units>}" << endl;
		out << "    Response is same as status." << endl;
		out << endl;

		out << url << "/?<b>update</b> <account_id> <password> {<order id> <new amount> <new rate>}" << endl;
		out << "    Response is same as status." << endl;
		out << endl;
		out << url << "/?<b>list_orders</b> <account_id> <password>" << endl;
		out << "    Receive a list of all enqueued orders with their remaining unmatched amounts." << endl;
		out << "    Columns in the response are: "; curex::cex::engine::unmatches_columns(out);
		out << endl;
		out << url << "/?<b>list_trades</b> <account_id> <password>" << endl;
		out << "    Receive a list of all finished trades." << endl;
		out << "    Columns in the response are: "; curex::cex::engine::trades_columns(out);
		out << endl;
		out << url << "/?<b>liquidity</b> <currency id> <currency id> [max depth]" << endl;
		out << "    Receive the list of available volume per exchange rate." << endl;
		out << "    If a value for 'depth' is specified only this many number of lines closer to each trading front are returned." << endl;
		out << "    Columns in the response are: "; curex::cex::engine::liquidity_columns(out);
		out << endl;
		out << url << "/?<b>update_order</b> <account id> <password> <order id> <new amount>" << endl;
		out << "    Updates the amount in queue of the specified order." << endl;
		out << "    If the amount is bigger than the actual order value a new order with be enqueued" << endl;
		out << "      such as the added value of the original order  plus the value of the new order sum the specified amount." << endl;
		out << "    If the amount is smaller than the actual order value it will be directly adjusted." << endl;
		out << "    If the amount is 0 the order will be cancelled." << endl;
		out << "    Funds are taken from or returned to the user wallet depending on the nature of the update." << endl;
		out << "    Response is same as status." << endl;
		out << endl;
		out << url << "/?<b>cancel_all_orders</b> <account id> <password>" << endl;
		out << "    Cancels all orders in all markets." << endl;
		out << "    Response is same as status." << endl;
		out << endl;
		out << url << "/?<b>withdraw</b> <account id> <password> <currency id> <amount>" << endl;
		out << "    Retrieve funds from your account and send them to the specified destination." << endl;
		out << "</pre>" << endl;


	}
      return true;
   }



	bool dumpall() {
      wchar_t cookieString[] = { '<', '"', 0x0440, 0x0443, 0x0441, 0x0441, 0x043a, 0x0438, 0x0439, '"', '>', ';', 0x0000 };
//      char cookieString[] = { '<', '"', 0x40, 0x43, 0x41, 0x41, 0x3a, 0x38, 0x39, '"', '>', ';', 0x00 };
//      char_t cookieString[] = { '<', '"', 0x0440, 0x0443, 0x0441, 0x0441, 0x043a, 0x0438, 0x0439, '"', '>', ';', 0x0000 };
      out << "Set-Cookie: echoCookie=" << encoding(URL) << cookieString << encoding(NONE) << "; path=/\n";
      out << "Content-Type: text/html; charset=utf-8\r\n\r\n";
      out << "<html><head><meta http-equiv='Content-Type' content='text/html; charset=utf-8' />";
      out << "<title>fastcgi++: Echo in UTF-8</title></head><body>";
      out << "<h1>Environment Parameters</h1>";
      out << "<p><b>FastCGI Version:</b> " << Protocol::version << "<br />";
      out << "<b>fastcgi++ Version:</b> " << version << "<br />";
      out << "<b>Hostname:</b> " << encoding(HTML) << environment().host << encoding(NONE) << "<br />";
      out << "<b>User Agent:</b> " << encoding(HTML) << environment().userAgent << encoding(NONE) << "<br />";
      out << "<b>Accepted Content Types:</b> " << encoding(HTML) << environment().acceptContentTypes << encoding(NONE) << "<br />";
      out << "<b>Accepted Languages:</b> " << encoding(HTML) << environment().acceptLanguages << encoding(NONE) << "<br />";
      out << "<b>Accepted Characters Sets:</b> " << encoding(HTML) << environment().acceptCharsets << encoding(NONE) << "<br />";
      out << "<b>Referer:</b> " << encoding(HTML) << environment().referer << encoding(NONE) << "<br />";
      out << "<b>Content Type:</b> " << encoding(HTML) << environment().contentType << encoding(NONE) << "<br />";
      out << "<b>Root:</b> " << encoding(HTML) << environment().root << encoding(NONE) << "<br />";
      out << "<b>Script Name:</b> " << encoding(HTML) << environment().scriptName << encoding(NONE) << "<br />";
      out << "<b>Request URI:</b> " << encoding(HTML) << environment().requestUri << encoding(NONE) << "<br />";
      out << "<b>Request Method:</b> " << encoding(HTML) << environment().requestMethod << encoding(NONE) << "<br />";
      out << "<b>Content Length:</b> " << encoding(HTML) << environment().contentLength << encoding(NONE) << "<br />";
      out << "<b>Keep Alive Time:</b> " << encoding(HTML) << environment().keepAlive << encoding(NONE) << "<br />";
      out << "<b>Server Address:</b> " << encoding(HTML) << environment().serverAddress << encoding(NONE) << "<br />";
      out << "<b>Server Port:</b> " << encoding(HTML) << environment().serverPort << encoding(NONE) << "<br />";
      out << "<b>Client Address:</b> " << encoding(HTML) << environment().remoteAddress << encoding(NONE) << "<br />";
      out << "<b>Client Port:</b> " << encoding(HTML) << environment().remotePort << encoding(NONE) << "<br />";
      out << "<b>If Modified Since:</b> " << encoding(HTML) << environment().ifModifiedSince << encoding(NONE) << "</p>";
      out << "<h1>Path Data</h1>";
      if(environment().pathInfo.size())
      {
         std::wstring preTab;
         for(Http::Environment<chartype>::PathInfo::const_iterator it=environment().pathInfo.begin(); it!=environment().pathInfo.end(); ++it)
         {
            out << preTab << encoding(HTML) << *it << encoding(NONE) << "<br />";
            preTab += L"&nbsp;&nbsp;&nbsp;";
         }
      }
      else
         out << "<p>No Path Info</p>";
      out << "<h1>GET Data</h1>";
      if(environment().gets.size())
         for(Http::Environment<chartype>::Gets::const_iterator it=environment().gets.begin(); it!=environment().gets.end(); ++it)
            out << "<b>" << encoding(HTML) << it->first << encoding(NONE) << ":</b> " << encoding(HTML) << it->second << encoding(NONE) << "<br />";
      else
         out << "<p>No GET data</p>";
      
      out << "<h1>Cookie Data</h1>";
      if(environment().cookies.size())
         for(Http::Environment<chartype>::Cookies::const_iterator it=environment().cookies.begin(); it!=environment().cookies.end(); ++it)
            out << "<b>" << encoding(HTML) << it->first << encoding(NONE) << ":</b> " << encoding(HTML) << it->second << encoding(NONE) << "<br />";
      else
         out << "<p>No Cookie data</p>";
      out << "<h1>POST Data</h1>";
      if(environment().posts.size())
      {
         for(Http::Environment<chartype>::Posts::const_iterator it=environment().posts.begin(); it!=environment().posts.end(); ++it)
         {
            out << "<h2>" << encoding(HTML) << it->first << encoding(NONE) << "</h2>";
            if(it->second.type==Http::Post<chartype>::form)
            {
               out << "<p><b>Type:</b> form data<br />";
               out << "<b>Value:</b> " << encoding(HTML) << it->second.value << encoding(NONE) << "</p>";
            }
            
            else
            {
               out << "<p><b>Type:</b> file<br />";
               out << "<b>Filename:</b> " << encoding(HTML) << it->second.filename << encoding(NONE) << "<br />";
               out << "<b>Content Type:</b> " << encoding(HTML) << it->second.contentType << encoding(NONE) << "<br />";
/*
               out << "<b>Size:</b> " << it->second.size << "<br />";
               out << "<b>Data:</b></p><pre>";
               out.dump(it->second.data.get(), it->second.size);

               out << "</pre>";
*/
            }
         }
      }
      else
         out << "<p>No POST data</p>";
      out << "</body></html>";
      return true;
  }
};

void error_log(const char* msg) {
   using namespace std;
   using namespace boost;
   static std::ofstream error;
   if(!error.is_open())
   {
      error.open("/tmp/errlog", ios_base::out | ios_base::app);
      error.imbue(locale(error.getloc(), new posix_time::time_facet()));
   }
   error << '[' << posix_time::second_clock::local_time() << "] " << msg << endl;
}

int main() {
   //engine::init(); 
   e=new engine("/var/cex",wcerr);
   e->add_sample_liquidity(wcerr);
   try {
      Fastcgipp::Manager<cexapi> fcgi;
      fcgi.handler();
      cerr << "curex api fast-cgi initiated normally" << endl;
   }
   catch(std::exception& ex) {
      error_log(ex.what());
   }
   delete e;
}

