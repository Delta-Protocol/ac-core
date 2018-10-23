#include "pairing.h"

#include <fstream>

#include <us/gov/auth/peer_t.h>

using namespace std;
using namespace us::wallet;

string pairing::device::m_default_name("my device");
string pairing::devices_t::m_not_found("not found");

pairing::pairing(const string& homedir):m_devices(homedir) {}

pairing::~pairing() {}

pairing::devices_t::devices_t(const string& home): m_home(home) {
    assert(!home.empty());
    load();
}

void pairing::devices_t::load() {
    lock_guard<mutex> lock(m_mx);
    load_();
}

void pairing::devices_t::save() const {
    lock_guard<mutex> lock(m_mx);
    save_();
}

void pairing::devices_t::load_() {
    clear();
    ifstream is(m_home+"/d");
    while (is.good()) {
        auto d=device::from_stream(is);
        if (!d.first) 
            break;
        if (!d.second.m_pub.is_valid()) 
            break;
        emplace(d.second.m_pub.hash(),d.second);
    }
}

void pairing::devices_t::save_() const {
    ofstream os(m_home+"/d");
    for (auto&i:*this) {
        i.second.to_stream(os);
        os << ' ';
    }
}

void pairing::device::to_stream(ostream&os) const {
    os << m_pub << ' ' << m_name;
}

pair<bool, pairing::device> pairing::device::from_stream(istream& is) {
    std::pair<bool,pairing::device> d;
    is >> d.second.m_pub;
    is >> d.second.m_name;
    d.first=is.good();
    return move(d);
}

void pairing::device::dump(ostream& os) const {
    os << m_pub << ' ' << m_name;
}

bool pairing::devices_t::authorize(const pub_t& p) const {
    lock_guard<mutex> lock(m_mx);
    if (empty()) {
        cout << "authorizing first device " << p << endl;
        const_cast<pairing::devices_t&>(*this).emplace(p.hash(),device(p,"First_seen_device"));
        save_();
        return true;
    }
    if (find(p.hash())!=end()) {
        cout << "found in the entry list  " << p << endl;
        return true;
    }
    cout << "not authorizing " << p << endl;
    return false;
}

string pairing::devices_t::pair_request() const { 
    return us::gov::auth::peer_t::get_random_message();
}

void pairing::devices_t::pair(const pub_t& pub, const string& name) {
    lock_guard<mutex> lock(m_mx);
    string n=name;
    if (n.empty()) 
        n=device::m_default_name;
    auto r=emplace(pub.hash(),device(pub,name));
    if (!r.second) 
        r.first->second.m_name=n;
    save_();
}

void pairing::devices_t::unpair(const pub_t& pub) {
    lock_guard<mutex> lock(m_mx);
    auto i=find(pub.hash());
    if (i==end()) 
        return;
    erase(i);
    save_();
}

const string& pairing::devices_t::get_name(const pub_t& pub) const {
    lock_guard<mutex> lock(m_mx);
    auto i=find(pub.hash());
    if (i==end()) 
        return m_not_found;
    return i->second.m_name;
}

void pairing::devices_t::dump(ostream& os) const {
    lock_guard<mutex> lock(m_mx);
    for (auto&i:*this) {
        i.second.dump(os);
        os << endl;
    }
    os << size() << " devices";
}
