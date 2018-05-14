#include "pairing.h"
#include <fstream>

typedef us::wallet::pairing c;
using namespace std;
using namespace us::wallet;

string c::devices_t::default_name("my device");

c::devices_t::devices_t(const string& home) {
    file=home+"/d";
    load();
}


void c::devices_t::load() {
	lock_guard<mutex> lock(mx);
    load_();
}

void c::devices_t::save() const {
	lock_guard<mutex> lock(mx);
    save_();
}

void c::devices_t::load_() {
    clear();
    ifstream is(home+"/d");
    while (is.good()) {
        device d=device::from_stream(is);
        emplace(d.pub.hash(),move(d));
    }
}

void c::devices_t::save_() const {
    ofstream os(home+"/d");
    for (auto&i:*this) {
        i.second.to_stream(os);
        os << ' ';
    }
}

void c::device::to_stream(ostream&os) const {
    os << pub << ' ' << name;
}

c::device c::device::from_stream(istream&is) {
    device d;
    pub_t pk;
    is >> d.pub;
    is >> d.name;
    return move(d);
}

void c::devices_t::pair(const pub_t& pub, const string& name) {
	lock_guard<mutex> lock(mx);
    emplace(pub.hash(),device(pub,name));
    save_();
}

void c::devices_t::unpair(const pub_t& pub) {
	lock_guard<mutex> lock(mx);
    auto i=find(pub.hash());
    if (i==end()) return;
    erase(i);
    save_();
}

const string& c::devices_t::get_name(const pub_t& pub) {
	lock_guard<mutex> lock(mx);
    auto i=find(pub.hash());
    if (i==end()) return default_name;
    return i->second.name;
}



