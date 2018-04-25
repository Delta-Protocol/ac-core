#include "args.h"

using namespace usgov;
using namespace std;
typedef usgov::args_t c;


template<>
string usgov::convert(const string& s) {
	return s;
}

