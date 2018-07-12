#include "args.h"

using namespace us;
using namespace std;
typedef us::args_t c;


template<> string us::convert(const string& s) {
	return s;
}

