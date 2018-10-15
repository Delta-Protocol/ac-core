#include "cfg1.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <fstream>

using namespace std;
using namespace us::gov::input;

cfg1::cfg1(const keys_t::priv_t& privk, const string& home): keys(privk), cfg0(home) {
    if (!keys.pub.valid) {
        cerr << "Invalid node pubkey" << endl;
        exit(1);
    }
}

cfg1::cfg1(const cfg1& other): keys(other.keys), cfg0(other) {
}

cfg1::~cfg1() {
}

pair<bool,cfg1::keys_t::priv_t> cfg1::load_priv_key(const string& home) {
    string keyfile=abs_file(home,"k");
    string pkb58;
    pair<bool,cfg1::keys_t::priv_t> r;
    {
        ifstream f(keyfile);
        if (!f.good()) {
            r.first=false;
            return move(r);
        }
        f >> r.second;
    }

    if (!crypto::ec::keys::verify(r.second)) {
        r.first=false;
        return move(r);
    }

    r.first=true;
    return move(r);
}

string cfg1::k_file(const string& home) {
    return abs_file(home,"k");
}

void cfg1::write_k(const string& home, const keys_t::priv_t& priv) {
    if (!ensure_dir(home)) {
         cerr << "Cannot create dir " << home << endl;
         exit(1);
    }

    ofstream f(k_file(home));
    f << priv << endl;
}

cfg1 cfg1::load(const string& home) {
    auto x=cfg0::load(home);

    string keyfile=k_file(home);
    if (!file_exists(keyfile)) {
        crypto::ec::keys k=crypto::ec::keys::generate();
        write_k(home,k.priv);
    }

    auto pk=load_priv_key(home);
    if (!pk.first) {
        cerr << "Invalid private key " << endl;
        exit(1);
    }

    return cfg1(pk.second,x.home);
}

