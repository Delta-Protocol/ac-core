#include "efp.h"
#include <cmath>
#include <algorithm>


using namespace curex::cex;
using namespace std;

double curex::cex::decode_dbl(const efp& v) {
	istringstream is(decode(v));
	double d=0;	
	is >> d;
	return d;
	//return ((double)v)/((double)one);
}

std::string curex::cex::decode_std(const efp& v) {
	std::ostringstream os;
	os << v;
	std::string s=os.str();
	if (s.size()<=curex::cex::precision) {
		s=std::string("0.")+std::string(curex::cex::precision-s.size(),'0')+s;
	}
	else {
		s.insert(s.size()-curex::cex::precision,1,'.');
	}
	return move(s);
}

curex::cex::string curex::cex::decode(const efp& v) {
	curex::cex::ostringstream os;
	os << v;
	curex::cex::string s=os.str();
	if (s.size()<=curex::cex::precision) {
		s=curex::cex::string(L"0.")+curex::cex::string(curex::cex::precision-s.size(),'0')+s;
	}
	else {
		s.insert(s.size()-curex::cex::precision,1,'.');
	}
	return move(s);
}

efp curex::cex::encode(const double& v) {
	return floor(v*one);
}
efp curex::cex::encode(string s) {
	if (s.empty()) return 0;
	int pos=-1;
	int i=0;
	for (auto h:s) {
		if (h=='.') {
			if (pos!=-1) return 0; //more than 1 dot
			pos=i;
			continue;	
		}
		if (h<'0' || h>'9') return 0; //not a digit
		++i;
	}
	if (pos==-1) { //no dot
		s+=string(curex::cex::precision,'0');
	}	
	else {
		int rpos=s.size()-pos;
//cout << "rpos=" << rpos << endl;
		if (rpos>curex::cex::precision+1) return 0; //more than 4 decimals
		s+=string(curex::cex::precision+1-rpos,'0');
		s.replace(pos,1,L""); //remove dot
	}
	istringstream is(s);
	efp r=0;
	is >> r;
	return r;
}

#include <cassert>
pair<efp,efp> curex::cex::encode(const double& net,const double& comm, const efp& tot) { //puede perder casi una diezmilesima (0.9999..) maximo 
	pair<efp,efp> r(encode(net),encode(comm));
	efp diff=tot-(r.first+r.second);
	if (r.second>100)
		r.second+=diff;
	else
		r.first+=diff;
	assert(r.first+r.second==tot);
	return move(r);
}


