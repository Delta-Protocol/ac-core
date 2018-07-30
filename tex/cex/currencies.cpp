#include "currencies.h"
#include <unordered_map>
#include <iostream>
#include <string>

using namespace curex::cex;

//https://en.wikipedia.org/wiki/ISO_4217


int eur::id=1;
string eur::iso=L"EUR"; 

int usd::id=2;
string usd::iso=L"USD"; 

int gbp::id=3;
string gbp::iso=L"GBP"; 

int aud::id=4;
string aud::iso=L"AUD"; 

std::unordered_map<id,string> _curstr={ 
	{eur::id,eur::iso},
	{usd::id,usd::iso},
	{gbp::id,gbp::iso},
	{aud::id,aud::iso}
};
std::unordered_map<string,id> _strcur={ 
	{eur::iso,eur::id},
	{usd::iso,usd::id},
	{gbp::iso,gbp::id},
	{aud::iso,aud::id}
};

bool curex::cex::is_valid(int cid) {
	return _curstr.find(cid)!=_curstr.end();
}
int curex::cex::strcur(const string& s) {
	auto i=_strcur.find(s);
	if (i==_strcur.end()) return 0;
	return i->second;
}
const string& curex::cex::curstr(int id) {
	return _curstr.find(id)->second;
}
void curex::cex::dump_currencies(const string& prefix, ostream& os) {
	for (auto& i: _curstr) {
		os << prefix << i.first << L" " << i.second << std::endl;
	}
}

