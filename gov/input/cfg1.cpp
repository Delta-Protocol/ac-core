#include "cfg1.h"
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
typedef us::gov::input::cfg1 c;

c::cfg1(const keys_t::priv_t& privk, const string& home): keys(privk), b(home) {
	if (!keys.pub.valid) {
		cerr << "Invalid node pubkey" << endl;
		exit(1);
	}
}

c::cfg1(const cfg1& other): keys(other.keys), b(other) {
}

c::~cfg1() {
}

pair<bool,c::keys_t::priv_t> c::load_priv_key(const string& home) {
            string keyfile=abs_file(home,"k");
            string pkb58;
                pair<bool,c::keys_t::priv_t> r;
                {
                    ifstream f(keyfile);
                    if (!f.good()) {
                        r.first=false;
                        return move(r);
                    } 
                    f >> r.second;
//                getline(f,pkb58);
                }
//                r.second=crypto::ec::keys::priv_t::from_b58(pkb58);
                if (!crypto::ec::keys::verify(r.second)) {
                    //mv k -> k.bad TODO
                    r.first=false;
                    return move(r);
                }
                r.first=true;
                return move(r);
}

string c::k_file(const string& home) {
    return abs_file(home,"k");
}

void c::write_k(const string& home, const keys_t::priv_t& priv) {
    if (!ensure_dir(home)) {
         cerr << "Cannot create dir " << home << endl;
         exit(1);
    }
     ofstream f(k_file(home));
     f << priv << endl;
}

c c::load(const string& home) {
        auto x=b::load(home);

            string keyfile=k_file(home);
//                cout << "Loading conf from " << keyfile << endl;
                if (!file_exists(keyfile)) {
                        cout << "Generating cryptographic keys..."; cout.flush();
                        crypto::ec::keys k=crypto::ec::keys::generate();
                        write_k(home,k.priv);
                        cout << "done." << endl;
                }
                auto pk=load_priv_key(home);
/*
                string pkb58;
                {
                ifstream f(keyfile);
                getline(f,pkb58);
                }
                auto pk=crypto::ec::keys::priv_t::from_b58(pkb58);
                if (!crypto::ec::keys::verify(pk)) {
                    cerr << "Invalid private key " << endl;
                    exit(1);
                }
*/
                if (!pk.first) {
                    cerr << "Invalid private key " << endl;
                    //mv k -> k.bad TODO
                    exit(1);
                }

       return c(pk.second,x.home);
}


