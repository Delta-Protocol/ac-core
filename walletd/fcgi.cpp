#include "fcgi.h"

using namespace us::wallet::w3api;
typedef us::wallet::w3api::fcgi_t c;

int c::count_reqs{0};
us::wallet::api* c::api{0};


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


void c::help(ostream& os) const {
    	static const string url="https://127.0.0.1/api";
		os << "Content-Type: text/html; charset=utf-8" << endl << endl;
		os << "<pre>" << endl;
		os << "US-Wallet functions" << endl;
		os << endl;
		os << url << "/?<b>nova new compartiment</b>" << endl;
		os << url << "/?<b>nova move &lt;compartiment&gt; &lt;item&gt; &lt;load/unload&gt; [&lt;send&gt;] </b>" << endl;
		os << url << "/?<b>nova track &lt;compartiment&gt; &lt;sensors_b58&gt; [&lt;send&gt;] </b>" << endl;
		os << url << "/?<b>nova query &lt;compartiment&gt;</b>" << endl;
		os << url << "/?<b>mempool</b>" << endl;

		os << endl;
		os << "</pre>" << endl;
}


Json::Value c::to_json(const string& s) const {
    istringstream is(s);
    Json::Value val;
    int i=0;
    while(is.good()) {
        string f;
        is >> f;
        if (!f.empty()) val[i++]=f;
    }
    return val;    
}

bool c::response() {
        ++count_reqs;
//out << environment().requestUri << endl;
//return true;
//out << "count requests " << count_reqs << " " << this << " " << this_thread::get_id()  << endl;
/*
	if (environment().requestUri.size()>100) {
		out << "request is too long to be legitimated. Don't be nasty, you could be banned soon." << endl;
		return true;
	}
*/
//	out << fixed << setprecision(curex::cex::precision);



	auto uri=uri_decode(environment().requestUri);
	istringstream is(uri);
	string command;
	is >> command;

    ostringstream os;
	if (command=="/api/?nova") {
    	is >> command;
    	if (command=="new") {
        	is >> command;
        	if (command=="compartiment") {
    			api->new_address(os);
            }
        }
	    else if (command=="track") {
            wallet::nova_track_input i;
        	is >> i.compartiment;
        	is >> i.data;
            string sendover;
            is >> sendover;
            i.sendover=sendover=="send";
            api->nova_track(i,os);
        }
	    else if (command=="move") {
            wallet::nova_move_input i;
            is >> i.compartiment;
            is >> i.item;
            string s;
            is >> s;
            bool ok{false};
            if (s=="load") {
                i.load=true;
                ok=true;
            }
            else if (s=="unload") {
                i.load=false;
                ok=true;
            }
            if (ok) {
                string sendover;
                is >> sendover;
                i.sendover=sendover=="send";
                api->nova_move(i,os);
            }
        }
	    else if (command=="query") {
            nova::hash_t compartiment;
            is >> compartiment;
            api->nova_query(compartiment,os);
        }
        else if (command=="mempool") {
            os << "nova mempool" << endl;
        }
	}
    string r=os.str();
    bool json=true;
	if (!r.empty()) {
        if (json) {
		    out << "Content-Type: application/json; charset=utf-8" << endl << endl;
            out << to_json(r) << endl;
        }
        else {
		    out << "Content-Type: text/plain; charset=utf-8" << endl << endl;
            out << r << endl;
        }
	}
    else {
        help(out);
    }

    return true;
}


