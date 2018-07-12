#include "args0.h"

using namespace us::gov::input;
using namespace std;
typedef us::gov::input::args_t c;


template<> string us::gov::input::convert(const string& s) {
	return s;
}


