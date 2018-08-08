#ifndef _CUREX_CEXR_
#define _CUREX_CEXR_

#include <string>
#include <vector>


namespace curex {

using namespace std;


class client {
public:
	client();
	~client();

	typedef unsigned long long id;
public:
	id new_account(const std::string& password);
	void deposit(id uid, const std::string& password,int curid,string amount);


	struct tradeinput {
		string id_of_client;
		string code;
		string amount;
		string cur;
		string rate;
		string rateunits;
	};

	string trade(const id& uid, const string& pwd, const vector<tradeinput>& v);
	string list_markets();

	struct market {
		string name;
		string weak_cur;
		string strong_cur;
		int weak_curid;
		int strong_curid;
		void dump(ostream&os) const;

	};
	vector<market> get_markets();

	string valid_rates(int cur1, int cur2);

	vector<string> get_valid_rates(int cur1, int cur2);

	static std::string urlapi;


	void *curl{0};

};




}

#endif

