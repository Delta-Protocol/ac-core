#ifndef USGOV_50d37207c1cd1fede08cc12a819a7a44a67234e5ad287e693cd167c2ee630227
#define USGOV_50d37207c1cd1fede08cc12a819a7a44a67234e5ad287e693cd167c2ee630227

#include <string>
#include <sstream>
#include <us/gov/cash.h>
#include <us/gov/crypto.h>

namespace us { namespace wallet {
using namespace std;

template<typename T>
T convert(const string& s) {
	T v;
	istringstream is(s);
	is >> v;
	return move(v);
}

template<> string convert(const string& s);
template<> us::gov::cash::tx::sigcode_t convert(const string& s);
template<> us::gov::crypto::ec::keys::priv_t convert(const string& s);



struct args_t {	
	args_t(int argc, char** argv):argc(argc), argv(argv) {
        //for (int i=0; i<argc; ++i) cout << argv[i] << endl;
	}
	template<typename T>
	T next() {
//cout << argc << " " << n << endl;
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


}}

#endif
