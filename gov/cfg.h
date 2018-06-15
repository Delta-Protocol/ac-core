#ifndef USGOV_cecc5cb611fac4022cc60e3a9caee608906e8b3256ea18a917c439b57efff781
#define USGOV_cecc5cb611fac4022cc60e3a9caee608906e8b3256ea18a917c439b57efff781

namespace us { namespace gov {
namespace filesystem {
using namespace std;

struct cfg {
	cfg();
	virtual ~cfg();

	static bool mkdir(const string& d);
	static bool file_exists(const string& f);
	static bool ensure_dir(const string& d);
	static bool exists_dir(const string& d);
	static string abs_file(const string& home, const string& fn);

};

}
}}

#endif

