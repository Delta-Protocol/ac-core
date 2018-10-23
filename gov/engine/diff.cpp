#include "diff.h"
#include "app.h"
#include<us/gov/likely.h>
#include <us/gov/crypto/base58.h>

#include <cassert>
#include <iomanip>
#include <vector>

using namespace us::gov::engine;
using namespace std;

typedef us::gov::engine::diff c;

namespace {
string to_string(const vector<unsigned char>& data) {
    ostringstream os;
    for (auto i:data) os << hex << setfill('0') << setw(2) << (int)i;
    return os.str();
}
}

string local_deltas::message_to_sign() const {
    ostringstream os;
    os << size() << " ";
    for (auto& i:*this) {
        os << i.first << " ";
        i.second->to_stream(os);
    }
    return os.str();
}

void local_deltas::to_stream(ostream& os) const {
    assert(!get_signature().empty());
    os << get_pubkey() << " " << get_signature() 
       << " " << message_to_sign() << " ";
}

local_deltas* local_deltas::from_stream(istream& is) {
    local_deltas* instance=new local_deltas();

    pubkey_t pk;
    is >> pk;
    instance->set_pubkey(pk);

    string sign;
    is >> sign;
    instance->set_signature(sign);

    if (unlikely(instance->get_signature().empty())) {
        delete instance;
        return 0;
    }
    int n;
    is >> n;
    if (unlikely(n<0)) {
        delete instance;
        return 0;
    }
    for (int i=0; i<n; ++i) {
        int appid;
        is >> appid;

        app::local_delta* v=app::local_delta::create(appid,is);
        if (v!=0) {
            instance->emplace(appid,v);
        }
    }
    return instance;
}

void diff::to_stream(ostream& os) const {
    os << m_prev << " ";
    os << size() << " ";
    for (auto& i:*this) {
        os << i.first << " ";
        i.second->to_stream(os);
    }
    os << m_proof_of_work.size() << " ";
    for (auto& i:m_proof_of_work) {
        os << i.first << " " << i.second << " ";
    }
}

string diff::parse_string() const {
    stringstream ss;
    this->to_stream(ss);
    return ss.str();
}

diff* diff::from_stream(istream& is) {
    diff* bl=new diff();
    is >> bl->m_prev;
    int n;
    is >> n;
    if (unlikely(n<0)) {
        delete bl;
        return 0;
    }
    for (int i=0; i<n; ++i) {
        int appid;
        is >> appid;

        app::delta* ag=app::delta::create(appid,is);
        if (ag!=0) {
            bl->emplace(appid,ag);
        }
    }

    is >> n;
    if (n<0) {
        delete bl;
        return 0;
    }
    for (int i=0; i<n; ++i) {
        pubkey_t::hash_t pubkeyh;
        is >> pubkeyh;
        uint64_t work;
        is >> work;
        bl->m_proof_of_work.emplace(pubkeyh,work);
    }
    return bl;
}

bool diff::allow(const local_deltas& g) { 
    lock_guard<mutex> lock(m_mx_proof_of_work);
    const auto& pubkeyh=g.get_pubkey().hash();
    auto i=m_proof_of_work.find(pubkeyh);
    if (i!=m_proof_of_work.end()) {
        return false;
    }
    m_proof_of_work.emplace(pubkeyh,0);
    return true;
}

uint64_t diff::add(int appid, app::local_delta* g) { 
    auto i=find(appid);
    if (unlikely(i==end())) {
        i=emplace(appid,app::delta::create(appid)).first;
    }
    return i->second->merge(g);
}

void diff::add(local_deltas* ld) {
    uint64_t work=0;
    {
        lock_guard<mutex> lock(m_mx);
        for (auto& i:*ld) {
            work+=add(i.first,i.second);
            i.second=0;
        }
    }
    {
        lock_guard<mutex> lock(m_mx_proof_of_work);
        m_proof_of_work.find(ld->get_pubkey().hash())->second=work;
    }
    delete ld;
}

void diff::end_adding() {
    lock_guard<mutex> lock(m_mx);
    for (auto& i:*this) {
        i.second->end_merge();
    }
    m_h=false;
}

const diff::hash_t& diff::hash() const {
    if (!m_h) {
        hasher_t hasher;
        ostringstream os;
        to_stream(os);
        hasher.write(os.str());
        hasher.finalize(m_hash_cached);
        m_h=true; 
    }
    return m_hash_cached;
}

diff::hash_t diff::hash(const string& content) {
    hash_t ans;
    hasher_t hasher;
    ostringstream os;
    hasher.write(content);
    hasher.finalize(ans);
    return ans;
}


