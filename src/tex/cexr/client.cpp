#include "client.h"
#include <string>
#include <curlpp/Easy.hpp>
#include <sstream>


using namespace curex;
using namespace std;

typedef curex::client c;

string c::urlapi = "https://trocavaluta.com/api";

c::client() {
	curl = curl_easy_init();
}
int writer(char *data, size_t size, size_t nitems, string* buffer_in) {
	if (buffer_in != 0) {
		buffer_in->append(data, size * nitems);
		return size * nitems;
	}
	return 0;
}

c::id c::new_account(const string& password) {
	string buffer;
	curl_easy_reset(curl);
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	ostringstream url;
	url << urlapi << "/?new_account " << password;
	curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
	CURLcode result=curl_easy_perform(curl);
	if(result != CURLE_OK) return 0;

	istringstream is(buffer);
	id ret=0;
	is >> ret;
	return ret;

	//cout << buffer << endl;
}

//https://trocavaluta.com/api/?trade_multiple <account_id> <password> {<yourid> <B|S> <amount> <currency> <rate> <rate units>}
string c::trade(const id& uid, const string& pwd, const vector<tradeinput>& v) {
	string buffer;
	curl_easy_reset(curl);
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	ostringstream url;
	url << urlapi << "/?trade " << uid << " " << pwd << " ";
	for (auto ti:v) {
		url << ti.id_of_client << " ";
		url << ti.code << " ";
		url << ti.amount << " ";
		url << ti.cur << " ";
		url << ti.rate << " ";
		url << ti.rateunits << " ";
	}
//cout << url.str() << endl;
	curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
	CURLcode result=curl_easy_perform(curl);
	if(result != CURLE_OK) return 0;

	return string(buffer);
}


string c::list_markets() {
	string buffer;
	curl_easy_reset(curl);
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	ostringstream url;
	url << urlapi << "/?list_markets";
	curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
	CURLcode result=curl_easy_perform(curl);
	if(result != CURLE_OK) return 0;

	return string(buffer);
}

//https://trocavaluta.com/api/?valid_rates_simple

string c::valid_rates(int cur1, int cur2) {
	string buffer;
	curl_easy_reset(curl);
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	ostringstream url;
	url << urlapi << "/?valid_rates_simple " << cur1 << " " << cur2;
	curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
	CURLcode result=curl_easy_perform(curl);
	if(result != CURLE_OK) return 0;

	return string(buffer);
}

void c::deposit(id uid, const std::string& pwd,int curid,string amount) {
	string buffer;
	curl_easy_reset(curl);
	curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
	ostringstream url;
	url << urlapi << "/?deposit " << uid << " " << pwd << " " << curid << " " << amount;
	curl_easy_setopt(curl, CURLOPT_URL, url.str().c_str());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);
	CURLcode result=curl_easy_perform(curl);
//	if(result != CURLE_OK) return 0;

//	return string(buffer);
}




#include <boost/algorithm/string.hpp>
using namespace boost::algorithm;


vector<string> c::get_valid_rates(int c1, int c2) {
	vector<string> v;
	string l=valid_rates(c1,c2);
	istringstream is(l);
	while(!is.eof()) {
		string line;
		getline(is,line);
		trim(line);
		if (line.empty()) continue;
		v.push_back(line);
	}
	return move(v);
}

vector<c::market> c::get_markets() {
	string l=list_markets();
	istringstream is(l);
	vector<market> ans;
	while(!is.eof()) {
		string line;
		getline(is,line);
		trim(line);
		if (line.empty()) continue;
		istringstream is2(line);
		market m;
		is2>>m.name;
		string c2;
		is2>>c2;
		{
		vector<string> strs;
		boost::split(strs,c2,boost::is_any_of("/"));
		istringstream is3(strs[0]);
		istringstream is4(strs[1]);
		is3>>m.weak_curid;
		is4>>m.strong_curid;
		}
		{
		vector<string> strs;
		boost::split(strs,m.name,boost::is_any_of("/"));
		istringstream is3(strs[0]);
		istringstream is4(strs[1]);
		is3>>m.weak_cur;
		is4>>m.strong_cur;
		}
		ans.emplace_back(move(m));
	}
	return move(ans);

}

void c::market::dump(ostream& os) const {
	os << name << " " << weak_cur << "(" << weak_curid << ") " << strong_cur << "(" << strong_curid << ")" << endl;

}




c::~client() {
}


