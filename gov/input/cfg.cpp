#include "cfg.h"
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
typedef us::gov::input::cfg c;

c::cfg(const keys_t::priv_t& privk, const string& home, vector<string>&& seed_nodes): b(privk, home), seed_nodes(seed_nodes) {
}

c::cfg(const cfg& other): b(other), seed_nodes(other.seed_nodes) {
}

c::~cfg() {
}


c c::load(const string& home) {
    //keys
    auto x=b::load(home); 

       string blocks_dir=abs_file(home,"blocks");
//        cout << "making sure dir for blocks exists" << endl;

        if (!ensure_dir(blocks_dir)) {
        cerr << "Cannot create blocks dir " << blocks_dir << endl;
        exit(1);
        }

        string locking_dir=abs_file(home,"locking");
            if (!ensure_dir(locking_dir)) {
            cerr << "Cannot create locking-programs dir " << locking_dir << endl;
            exit(1);
        }


    //seed nodes 
        vector<string> addrs;
                string seeds_file=abs_file(home,"nodes.manual");
  //              cout << "reading " << seeds_file << endl;
                ifstream f(seeds_file);
                while(f.good()) {
                        string addr;
                        getline(f,addr);
                        if (addr.empty()) continue;
                        addrs.push_back(addr);
                }
//                cout << "loaded ip address of " << addrs.size() << " seed nodes" << endl;

            return cfg(x.keys.priv,x.home,move(addrs));
}

