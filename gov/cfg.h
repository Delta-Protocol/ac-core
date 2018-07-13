#ifndef USGOV_cecc5cb611fac4022cc60e3a9caee608906e8b3256ea18a917c439b57efff781
#define USGOV_cecc5cb611fac4022cc60e3a9caee608906e8b3256ea18a917c439b57efff781

#include <string>
#include <fstream>
#include "crypto.h"

namespace us { namespace gov {
namespace filesystem {
using namespace std;

struct cfg {
	typedef crypto::ec::keys keys_t;
	//cfg();
	virtual ~cfg();
       cfg(const keys_t::priv_t& privk, const string& home);
        cfg(const cfg& other);


	static bool mkdir(const string& d);
	static bool file_exists(const string& f);
	static bool ensure_dir(const string& d);
	static bool exists_dir(const string& d);
	static string abs_file(const string& home, const string& fn);
	static void mkdir_tree(string sub, string dir);

        static cfg load(const string& home);
        string home;
        keys_t keys;

};

}
}}

#endif

