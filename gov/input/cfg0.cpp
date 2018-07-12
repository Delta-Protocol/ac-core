#include "cfg0.h"
#include <string.h> //strerror
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <cassert>

using namespace std;
using namespace us::gov::input;
typedef us::gov::input::cfg0 c;

c::cfg0(const string& home): home(home) {
}

c::cfg0(const cfg0& other): home(other.home) {
}

c::~cfg0() {
}

bool c::mkdir(const string& d) {
	int e = ::mkdir(d.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if (e==-1) cerr << errno << " " << strerror(errno) << endl;
	return e!=-1;
}

bool c::file_exists(const string& f) {
    struct stat s;
    if (stat(f.c_str(), &s)==0) return S_ISREG(s.st_mode);
    return false;
}

#include "iostream"
#include "string"
#include "sys/types.h"
#include "sys/stat.h"

void c::mkdir_tree(string sub, string dir) {
	if (sub.length() == 0)
	return;

	int i=0;

	for (i; i < sub.length(); i++) {
		dir += sub[i];
		if (sub[i] == '/')
		break;
	}

	::mkdir(dir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	if (i+1 < sub.length())	mkdir_tree(sub.substr(i+1), dir);
}


bool c::ensure_dir(const string& d) {
	if (!exists_dir(d)) {
	    mkdir_tree(d,"");
		if (!exists_dir(d)) {
	           cerr << "Error creating directory " << d << endl;
			assert(false);
		}
	}
	return true;
}

bool c::exists_dir(const string& d) {
    struct stat s;
    if (stat(d.c_str(), &s)==0) return S_ISDIR(s.st_mode);
    return false;
}

string c::abs_file(const string& home, const string& fn) { 
	return home+"/"+fn;
}

c c::load(const string& home) {
    if (!ensure_dir(home)) {
         cerr << "Cannot create home dir " << home << endl;
         exit(1);
    }
    return c(home);
}


