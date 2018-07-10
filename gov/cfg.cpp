#include "cfg.h"
#include "crypto.h"
#include <string.h> //strerror
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <cstdlib>
#include <iostream>
#include <sys/types.h>
#include <cassert>

using namespace std;
using namespace us::gov::filesystem;
typedef us::gov::filesystem::cfg c;

c::cfg(const keys_t::priv_t& privk, const string& home): keys(privk), home(home) {
}

c::cfg(const cfg& other): keys(other.keys), home(other.home) {
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
    if (stat(f.c_str(), &s)==0) return S_ISREG(s.st_mode);
    return false;
}

#include "iostream"
#include "string"
#include "sys/types.h"
#include "sys/stat.h"

using namespace std;

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
/*
	    const int dir_err = mkdir("foo", S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

            ostringstream cmd;
            cmd << "mkdir -p " << d;
	        const int dir_err = system(cmd.str().c_str());
        	if (dir_err == -1) {
	           cerr << "Error creating directory " << d << endl;
        	   return false;
	        }
		return true;
*/
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
            string keyfile=abs_file(home,"k");
                cout << "Loading conf from " << keyfile << endl;
                if (!file_exists(keyfile)) {
                        cout << "Generating cryptographic keys..."; cout.flush();
                        crypto::ec::keys k=crypto::ec::keys::generate();
                        ofstream f(keyfile);
                        f << k.priv.to_b58() << endl;
                        cout << "done." << endl;
                }
                string pkb58;
                {
                ifstream f(keyfile);
                getline(f,pkb58);
                }
                auto pk=crypto::ec::keys::priv_t::from_b58(pkb58);
                if (!crypto::ec::keys::verify(pk)) {
                    cerr << "Invalid private key " << endl;
                    //mv k -> k.bad TODO
                    exit(1);
                }

            return cfg(pk,home);
}


