#include "cfg0.h"

#include <sys/stat.h>
#include <cassert>
#include <cerrno>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>

using namespace std;
using namespace us::gov::input;

cfg0::cfg0(const string& home): home(home) {
}

cfg0::cfg0(const cfg0& other): home(other.home) {
}

cfg0::~cfg0() {
}

bool cfg0::is_big_endian() {
    union {
        uint16_t i;
        char c[2];
    } bint = {0x0102};

    return bint.c[0] == 1; 
}

void cfg0::check_platform() {
    if (is_big_endian()) {
        cerr << "This program cannot run on big-endian systems." << endl;
        exit(1);
    }
}

bool cfg0::mkdir(const string& d) {
    int e = ::mkdir(d.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (e==-1) cerr << errno << " " << strerror(errno) << endl;
    return e!=-1;
}

bool cfg0::file_exists(const string& f) {
    struct stat s;
    if (stat(f.c_str(), &s)==0) return S_ISREG(s.st_mode);
    return false;
}

void cfg0::mkdir_tree(string sub, string dir) {
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

bool cfg0::ensure_dir(const string& d) {
    if (!exists_dir(d)) {
        mkdir_tree(d,"");
        if (!exists_dir(d)) {
            cerr << "Error creating directory " << d << endl;
            assert(false);
        }
    }
    return true;
}

bool cfg0::exists_dir(const string& d) {
    struct stat s;
    if (stat(d.c_str(), &s)==0) return S_ISDIR(s.st_mode);
    return false;
}

string cfg0::abs_file(const string& home, const string& fn) {
    return home+"/"+fn;
}

cfg0 cfg0::load(const string& home) {
    check_platform();
    if (!ensure_dir(home)) {
         cerr << "Cannot create home dir " << home << endl;
         exit(1);
    }

    return cfg0(home);
}

