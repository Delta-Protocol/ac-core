#include "fcgi.h"

using namespace us::wallet::w3api;
typedef us::wallet::w3api::fcgi_t c;

int c::count_reqs{0};

template<typename T>
std::basic_string<T> uri_decode(const std::basic_string<T>& in) {
  std::basic_string<T> out;
  out.clear();
  out.reserve(in.size());
  for (std::size_t i = 0; i < in.size(); ++i) {
    if (in[i] == L'%') {
      if (i + 3 <= in.size()) {
        int value;
        std::basic_istringstream<T> is(in.substr(i + 1, 2));
        if (is >> std::hex >> value) {
          out += static_cast<T>(value);
          i += 2;
        }
        else {
      out.clear();
          return out;
        }
      }
      else {
      out.clear();
      return out;
      }
    }
    else if (in[i] == L'+') {
      out += L' ';
    }
    else {
      out += in[i];
    }
  }
  return move(out);
}


bool c::response() {
        ++count_reqs;
//out << environment().requestUri << endl;
//out << "count requests " << count_reqs << " " << this << " " << this_thread::get_id()  << endl;
/*
	if (environment().requestUri.size()>100) {
		out << "request is too long to be legitimated. Don't be nasty, you could be banned soon." << endl;
		return true;
	}
*/
//	out << fixed << setprecision(curex::cex::precision);

	static const string url="https://127.0.0.1/api";

	auto uri=uri_decode(environment().requestUri);
	istringstream is(uri);
	string command;
	is >> command;

	if (command!="/api") {
		out << "Content-Type: text/plain; charset=utf-8" << endl << endl;
	}
	if (command=="/api/?trade") {
		//ticket_4 S 101 USD 1.4750 AUD/USD ticket_3 S 120 USD 1.4750 AUD/USD ticket_2 B 654.8977 AUD 1.4750 AUD/USD ticket_1 B 33 AUD 1.4750 AUD/USD
/*
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
*/
	}
	else {
		out << "Content-Type: text/html; charset=utf-8" << endl << endl;
		out << "<pre>" << endl;
		out << "US.GOV." << endl;
		out << "Wallet functions" << endl;
		out << endl;
		out << url << "/?<b>new_wallet</b> <account_id> <password> <currency id>" << endl;
		out << "    Creates a new wallet to store money in the specified currency" << endl;
		out << endl;
		out << "</pre>" << endl;


	}
      return true;
}


