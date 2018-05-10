#ifndef USGOV_161484e3427dab1da17e7cc5b457e84c200ad9cc0da7e7d690dd3cef49ca3c07
#define USGOV_161484e3427dab1da17e7cc5b457e84c200ad9cc0da7e7d690dd3cef49ca3c07
/*
#include <vector>

namespace us { namespace gov {
namespace blockchain {

using namespace std;

struct series: vector<diff> {
	static constexcpr int reduction_factor{16};
	series(): higher_period(0) {
		reserve(reduction_factor+1);
	}
	~series() {
		delete higher_period;
	}
	void add(diff d) { //called once per minute
		emplace_back(d);
		if (size()>reduction_factor) { //16 reduction factor
			if (higher_period==0) {
				higher_period=new series();
			}
			higher_period->add(reduce());
		}
	}
	diff reduce() {
		diff d;
		for (auto& i:*this) {
			d+=i;
		}
		clear();
		return d;
	}
	diff get_db() {
		diff db;
		if (higher_period!=0) {
			db=higher_period->get_db();
		}
		db+=reduce();
		return db;		
	}
	series* higher_period;
};


}}
}
*/

#endif
