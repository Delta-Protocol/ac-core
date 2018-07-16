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
}

c::cfg1(const cfg1& other): keys(other.keys), b(other) {
}

c::~cfg1() {
}

c c::load(const string& home) {
        auto x=b::load(home);

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


/*
        string blocks_dir=abs_file(home,"blocks");
        cout << "making sure dir for blocks exists" << endl;
        if (!ensure_dir(blocks_dir)) {
        cerr << "Cannot create blocks dir " << blocks_dir << endl;
        exit(1);
        }

        string locking_dir=abs_file(home,"locking");
            if (!ensure_dir(locking_dir)) {
            cerr << "Cannot create locking-programs dir " << locking_dir << endl;
            exit(1);
        }
*/

       return c(pk,x.home);
}


