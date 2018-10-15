#include "cfg.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <fstream>

#include "us/gov/crypto/ec.h"

using namespace std;
using namespace us::gov::input;

cfg::cfg(const keys_t::priv_t& privk, const string& home, vector<string>&& seed_nodes)
:cfg1(privk, home), seed_nodes(seed_nodes) {
}

cfg::cfg(const cfg& other): cfg1(other), seed_nodes(other.seed_nodes) {
}

cfg::~cfg() {
}

cfg cfg::load(const string& home) {
    auto x=cfg1::load(home);

    string blocks_dir=abs_file(home,"dfs");
    if (!ensure_dir(blocks_dir)) {
        cerr << "Cannot create blocks dir " << blocks_dir << endl;
        exit(1);
    }

    string locking_dir=abs_file(home,"locking");
    if (!ensure_dir(locking_dir)) {
        cerr << "Cannot create locking-programs dir " << locking_dir << endl;
        exit(1);
    }

    vector<string> addrs;
    string seeds_file=abs_file(home,"nodes.manual");
    if (!file_exists(seeds_file)) {
        ofstream f(seeds_file);
        f << '\n';
    }

    ifstream f(seeds_file);
    while(f.good()) {
        string addr;
        getline(f,addr);
        if (addr.empty()) continue;
            addrs.push_back(addr);
    }

    return cfg(x.keys.priv,x.home,move(addrs));
}

