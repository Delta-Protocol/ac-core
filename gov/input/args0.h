#ifndef USGOV_46373a79c914cc1afc4dcf1f5c83a282d35ddf519feb546780be965072e21ade
#define USGOV_46373a79c914cc1afc4dcf1f5c83a282d35ddf519feb546780be965072e21ade

#include <string>
#include <sstream>

namespace us { namespace gov {
namespace input {

using namespace std;

template<typename T>
T convert(const string& s) {
	T v;
	istringstream is(s);
	is >> v;
	return move(v);
}

template<> string convert(const string& s);


struct args_t {	
	args_t(int argc, char** argv):argc(argc), argv(argv) {
	}
	template<typename T>
	T next() {
		if (n>=argc) {
			return T();
		}
		string i=argv[n++];
		return convert<T>(i);
	}
	template<typename T>
	T next(const T& default_value) {
		if (n>=argc) {
			return default_value;
		}
		string i=argv[n++];
		return convert<T>(i);
	}
    int args_left() const {
        return argc-n;
    }
	int argc;
	char** argv;
	int n{1};
};


}
}}

#endif
