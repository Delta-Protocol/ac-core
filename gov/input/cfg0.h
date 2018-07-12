#ifndef USGOV_cecc5cb611fac4022cc60e3a9caee608906e8b3256ea18a917c439b57efff781
#define USGOV_cecc5cb611fac4022cc60e3a9caee608906e8b3256ea18a917c439b57efff781

#include <string>

namespace us { namespace gov { 
namespace input {
using namespace std;

struct cfg0 {
	virtual ~cfg0();
       cfg0(const string& home);
       cfg0(const cfg0& other);


	static bool mkdir(const string& d);
	static bool file_exists(const string& f);
	static bool ensure_dir(const string& d);
	static bool exists_dir(const string& d);
	static string abs_file(const string& home, const string& fn);
	static void mkdir_tree(string sub, string dir);

        static cfg0 load(const string& home);
        string home;

};

}
}}
                                                                

#endif

