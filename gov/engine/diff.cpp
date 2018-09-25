#include "diff.h"
#include "app.h"
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
	assert(!signature.empty());
	os << pubkey << " " << signature << " " << message_to_sign() << " ";
}

local_deltas* local_deltas::from_stream(istream& is) {
	local_deltas* instance=new local_deltas();
	is >> instance->pubkey;
	is >> instance->signature;
	if (unlikely(instance->signature.empty())) {
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

#include<us/gov/likely.h>
void c::to_stream(ostream& os) const {
	os << prev << " ";
	os << size() << " ";
	for (auto& i:*this) {
		os << i.first << " ";
		i.second->to_stream(os);
	}
	os << proof_of_work.size() << " ";
	for (auto& i:proof_of_work) {
		os << i.first << " " << i.second << " ";
	}
}

string c::parse_string() const {
    stringstream ss;
    this->to_stream(ss);
    return ss.str();
}

c* c::from_stream(istream& is) {
	c* bl=new c();
	is >> bl->prev;
//cout << "READING prev " << bl->prev << endl;
//	if (unlikely(bl->prev.is_zero())) return 0;
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
		bl->proof_of_work.emplace(pubkeyh,work);
	}

	return bl;
}

bool c::allow(const local_deltas& g) { 
	lock_guard<mutex> lock(mx_proof_of_work);
	const auto& pubkeyh=g.pubkey.hash();
	auto i=proof_of_work.find(pubkeyh);
	if (i!=proof_of_work.end()) {
		return false;
	}
	proof_of_work.emplace(pubkeyh,0);
    return true;
}

uint64_t c::add(int appid, app::local_delta* g) { //private, not protected by mutex, see add fn below
	auto i=find(appid);
	if (unlikely(i==end())) {
		i=emplace(appid,app::delta::create(appid)).first;
	}
	return i->second->merge(g);
}

void c::add(local_deltas* ld) {
	uint64_t work=0;
	{
	lock_guard<mutex> lock(mx);
	for (auto& i:*ld) {
		work+=add(i.first,i.second);
		i.second=0;
	}
	}

	{
	lock_guard<mutex> lock(mx_proof_of_work);
	proof_of_work.find(ld->pubkey.hash())->second=work;
	}

	delete ld;
}

void c::end_adding() {
	lock_guard<mutex> lock(mx);
	for (auto& i:*this) {
		i.second->end_merge();
	}
	h=false; // invalidate cached hash
}

#include <us/gov/crypto/base58.h>

const c::hash_t& c::hash() const {
	if (!h) {
		hasher_t hasher;
		ostringstream os;
		to_stream(os);
		hasher.write(os.str());
		hasher.finalize(hash_cached);
		h=true; //cached hash is fine
	}
	return hash_cached;
}

c::hash_t c::hash(const string& content) {
	hash_t ans;
	hasher_t hasher;
	ostringstream os;
	hasher.write(content);
	hasher.finalize(ans);
	return ans;
}


