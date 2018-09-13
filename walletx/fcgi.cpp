#ifdef FCGI
#include "fcgi.h"
#include <us/wallet/json/daemon_api.h>
#include <unordered_map>
#include <us/gov/input/convert.h>

using namespace us::wallet::w3api;
typedef us::wallet::w3api::fcgi_t c;

int c::count_reqs{0};
us::wallet::daemon_api* c::api{0};

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
		os << "us-wallet functions" << endl;
		os << endl;

    os << "Wallet" << endl;
#include <us/api/apitool_generated__links_wallet_fcgi_query_strings>
    os << endl;
    os << "Pairing" << endl;
#include <us/api/apitool_generated__links_pairing_fcgi_query_strings>
    os << endl;

//		os << url << "/?<b>cmd=balance&detailed=&lt;1|0&gt;</b>" << endl;
/*
		os << url << "/?<b>app=nova&cmd=new_compartiment</b>" << endl;
		os << url << "/?<b>app=nova&cmd=move&compartiment=&lt;compartiment&gt;&item=&lt;item&gt;&action=&lt;load/unload&gt;&send=&lt;1|0&gt; </b>" << endl;
		os << url << "/?<b>app=nova&cmd=track&compartiment=&lt;compartiment&gt;&sensors=&lt;sensors_b58&gt;&send=&lt;1|0&gt; </b>" << endl;
		os << url << "/?<b>app=nova&cmd=query&compartiment=&lt;compartiment&gt;</b>" << endl;
		os << url << "/?<b>app=nova&cmd=query&item=&lt;item&gt;</b>" << endl;
		os << url << "/?<b>app=nova&cmd=mempool</b>" << endl;
*/
		os << endl;
		os << "</pre>" << endl;
}

#include "json.h"

/*
Json::Value c::to_json(const string& r,const string& cmd) {

    if (cmd=="balance") return json::convert_response_balance(r);
    if (cmd=="balance_detailed") return json::convert_response_balance_detailed(r);
/ *
    if (cmd=="new_compartiment") return json::convert_response_new_compartiment(r);
    if (cmd=="move") return json::convert_response_move(r);
    if (cmd=="track") return json::convert_response_track(r);
    if (cmd=="query") return json::convert_response_query(r);
    if (cmd=="mempool") return json::convert_response_mempool(r);
* /
    Json::Value err;
    err["error"]="unknown command";
    return err;
}
*/
pair<string,string> split(const string& s, char c) {
	for (int i=0; i<s.size(); ++i) {
		if (s[i]==c) {
			return make_pair(s.substr(0,i),s.substr(i+1));
		}
	}
	return make_pair(s,"");
}
vector<string> split2(const string& s, char c) {
	vector<string> v;
	int previ=0;
	for (int i=0; i<s.size(); ++i) {
		if (s[i]==c) {
			v.push_back(s.substr(previ,i-previ));
			previ=i+1;
		}
	}
	if (previ<s.size()) v.push_back(s.substr(previ));
	return v;
}

using namespace us::gov::input;


struct args_t:unordered_map<string,string> {
    const string& get(const string& param) const;
    template<typename T>
    T get(const string& param) {
        auto i=find(param);
        if (i==end()) {
            return T();
        }
        return convert<T>(i->second);
    }
    template<typename T>
    T get(const string& param, const T& default_value) {
        auto i=find(param);
        if (i==end()) {
            return default_value;
        }
        return convert<T>(i->second);
    }
};

args_t parse_uri(const string& uri) {
	args_t m;
	auto i=uri.find('?');
	if (i==string::npos) return m;
	auto p=split2(&uri[i+1],'&'); //vector<str> "para=value"
	for (auto&i:p) {
		auto v=split(i,'='); //pair<str,str>
		m.emplace(v);
	}
	return m;
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

//https://10.84.172.95/api?app=nova&cmd=track&compartiment=&sensors=&send=1

	auto uri=uri_decode(environment().requestUri);
/*
out << "Content-Type: text/plain; charset=utf-8" << endl << endl;

out << uri << endl;
	for (auto& i:m) {
		out << i.first << " " << i.second << endl;

	}

	return true;
*/
        ostringstream os;

	auto args=parse_uri(uri);
//	if (==m.end()) {help(out); return true;}
	//string app=n->second;

//	istringstream is(uri);
//	string command;
//	is >> command;
//    istringstream is("");
    string cmd=args.get<string>("cmd");
  	if (cmd=="balance") {
       string detailed=args.get<string>("detailed");
        bool b=detailed=="1";
  	   api->balance(b,os);
    }
/*
	if (app=="nova") {
	++n;
	if (n==m.end()) {help(out); return true;}
	cmd=n->second;
  	if (cmd=="new_compartiment") {
   	    api->new_address(os);
    }
	else if (cmd=="track") {
        wallet::nova_track_input i;
		++n;if (n==m.end()) {help(out); return true;}
     	i.compartiment=nova::hash_t::from_b58(n->second);
		++n;if (n==m.end()) {help(out); return true;}
       	i.data=n->second;
		++n;if (n==m.end()) {help(out); return true;}
        string sendover;
        sendover=n->second;
        i.sendover=sendover=="1";
        api->nova_track(i,os);
    }
    else if (cmd=="move") {
        wallet::nova_move_input i;
		++n;if (n==m.end()) {help(out); return true;}
       	i.compartiment=nova::hash_t::from_b58(n->second);
		++n;if (n==m.end()) {help(out); return true;}
       	i.item=n->second;
		++n;if (n==m.end()) {help(out); return true;}
        string s;
        s=n->second;
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
            ++n;if (n==m.end()) {help(out); return true;}
            string sendover;
            sendover=n->second;
            i.sendover=sendover=="1";
            api->nova_move(i,os);
        }
    }
    else if (cmd=="query") {
		++n;if (n==m.end()) {help(out); return true;}
	if (n->first=="compartiment") {
	        nova::hash_t compartiment;
     		compartiment=nova::hash_t::from_b58(n->second);
	        api->nova_query(compartiment,os);
	}
	else {
		string item=n->second;
		api->nova_query(item,os);
	}
    }
    else if (cmd=="mempool") {
        api->nova_mempool(os);
    }




    }
*/
    string r=os.str();
//    bool json=true;
	if (!r.empty()) {
        if (dynamic_cast<json::daemon_api*>(api)!=0) {
		    out << "Content-Type: application/json; charset=utf-8" << endl << endl;
        }
        else {
		    out << "Content-Type: text/plain; charset=utf-8" << endl << endl;
        }
        out << r << endl;
	}
    else {
        help(out);
    }

    return true;
}


#endif

