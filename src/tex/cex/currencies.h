#ifndef CUREX_CEX_CURRENCIES_H
#define CUREX_CEX_CURRENCIES_H
#include "cex.h"

namespace curex { namespace cex {


struct eur {
	static int id;
	static string iso; 
};
struct usd {
	static int id;
	static string iso; 
};
struct gbp {
	static int id;
	static string iso; 
};
struct aud {
	static int id;
	static string iso; 
};

bool is_valid(int cid);
const string& curstr(int id);
int strcur(const string& s);

void dump_currencies(const string& prefix, ostream& os);



}}

#endif

