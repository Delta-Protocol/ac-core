#include "cfg.h"
//#include "crypto.h"
#include <string.h> //strerror
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <cstdlib>
#include <iostream>

using namespace std;
using namespace us::gov::filesystem;
typedef us::gov::filesystem::cfg c;

c::cfg() {
}

c:: ~cfg() {
}

bool mkdir(const string& d) {
	int e = ::mkdir(d.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (e==-1) cerr << errno << " " << strerror(errno) << endl;
	return e!=-1;
}

 bool c::file_exists(const string& f) {
    struct stat s;
    stat(f.c_str(), &s);
    return S_ISREG(s.st_mode);
}

bool c::ensure_dir(const string& d) {
	if (!exists_dir(d)) {
            ostringstream cmd;
            cmd << "mkdir -p " << d;
	        const int dir_err = system(cmd.str().c_str());
        	if (dir_err == -1) {
	           cerr << "Error creating directory " << d << endl;
        	   return false;
	        }
		return true;
	}
	return true;
}

bool c::exists_dir(const string& d) {
    struct stat s;
    stat(d.c_str(), &s);
    return S_ISDIR(s.st_mode);
}

string c::abs_file(const string& home, const string& fn) { 
	return home+"/"+fn;
}

