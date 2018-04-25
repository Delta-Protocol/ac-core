#ifndef USGOV_cecc5cb611fac4022cc60e3a9caee608906e8b3256ea18a917c439b57efff781
#define USGOV_cecc5cb611fac4022cc60e3a9caee608906e8b3256ea18a917c439b57efff781

#include "crypto.h"
#include <fstream>
#include <sys/stat.h>

namespace usgov {
namespace filesystem {
using namespace std;

struct cfg {
	cfg() {
	}
	virtual ~cfg() {
	}

	static bool mkdir(const string& d) {
		int e = ::mkdir(d.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		if (e==-1) cerr << errno << " " << strerror(errno) << endl;
		return e!=-1;
	}
	static bool file_exists(const string& f) {
	    struct stat s;
	    stat(f.c_str(), &s);
	    return S_ISREG(s.st_mode);
	}
	static bool ensure_dir(const string& d) {
		if (!exists_dir(d)) {
			return mkdir(d);
		}
	    return true;
	}
	static bool exists_dir(const string& d) {
	    struct stat s;
	    stat(d.c_str(), &s);
	    return S_ISDIR(s.st_mode);
	}

	static string abs_file(const string& home, const string& fn) { return home+"/"+fn; }

};

}
}

#endif

