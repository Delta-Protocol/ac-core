#include "rate.h"
/*
using namespace curex::cex;
using namespace std;
typedef curex::cex::rate c;

	c::rate(): value(10000) { //adjust to 5 digits
	}
	c::rate(const efp& v): value(v) { //adjust to 4 digits
	}
	c::rate(const rate& other): value(other.value) {
	}

	rate& c::operator = (const rate& other) {
		value=other.value;
		return *this;
	}	
	rate& c::operator = (rate&& other) {
		value=move(other.value);
		return *this;
	}	

	bool c::make_valid(efp& v) {
		if (!is_valid(v)) return false;
		efp r=v;
		r%=rate_multiples;
		if (r==1) --v;
		if (r==-1) ++v;
		return true;
	}
	bool c::is_valid(const efp& v) {
		if (v<1) return false;
		efp r=v;
		r%=rate_multiples;
		if (r==0) return true;
		if (r==1) return true;
		if (r==-1) return true;
		return false;
	}

	  bool c::operator !=(rate const& t2) const {
	    return value!=t2.value;
	  }

	  bool c::operator <(const rate& other) const {
	    return value<other.value;
	  }
	*/

