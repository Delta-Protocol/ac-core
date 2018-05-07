#include "diff.h"
#include "app.h"
#include <cassert>

using namespace usgov::blockchain;
using namespace std;

typedef usgov::blockchain::diff c;


#include <iomanip>
#include <vector>
namespace {
string to_string(const vector<unsigned char>& data) {
	ostringstream os;
	for (auto i:data) os << hex << setfill('0') << setw(2) << (int)i;
	return os.str();
}
}


//c::bucket c::empty_bucket;
/*
void c::bucket::to_stream(ostream& os) const {
	os << size() << endl;
	for (auto& i:*this) {
		os << i.first << endl;
		i.second->to_stream(os);
	}
}

c::bucket::appguts_by_pubkey(istream& is) {
	int n;
	is >> n;
	for (int i=0; i<n; ++i) {
		string pubkey;
		is >> pubkey;
		app_gut* g=app_gut::create(is);
		if (g==0) continue; //app no longer recognized
		emplace(pubkey,g);
	}
}

c::bucket::~appguts_by_pubkey() {
	for (auto& i:*this) delete i.second;
}
*/

string miner_gut::message_to_sign() const {
	ostringstream os;
	os << size() << " ";
	for (auto& i:*this) {
		os << i.first << " ";
		i.second->to_stream(os);
	}
	return os.str();
}

void miner_gut::to_stream(ostream& os) const {
	assert(!signature.empty());
	os << pubkey << " " << signature << " " << message_to_sign() << " ";
}

miner_gut* miner_gut::from_stream(istream& is) {
	miner_gut* instance=new miner_gut();
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
	//instance->reserve(n);
	for (int i=0; i<n; ++i) {
		int appid;
		is >> appid;

		local_delta* v=local_delta::create(appid,is);
		if (v!=0) {
			instance->emplace(appid,v);
		}
	}
	return instance;
}

/*
const c::bucket& c::get_app_guts(int id) const {
	auto i=find(id);
	if (i==end()) return empty_bucket;
	return i->second;
}
*/

#include<gov/likely.h>
void c::to_stream(ostream& os) const {
	os << prev << " ";
//cout << "WRITING prev " << prev << endl;
	os << size() << " ";
	for (auto& i:*this) {
		os << i.first << " ";
		i.second->to_stream(os);
	}
	os << proof_of_work.size() << " ";
	for (auto& i:proof_of_work) {
//cout << "WRITING pow.first " << i.first << endl;
		os << i.first << " " << i.second << " ";
	}
}

c* c::from_stream(istream& is) {
	c* bl=new c();
	is >> bl->prev;
//cout << "READING prev " << bl->prev << endl;
//	if (unlikely(bl->prev.is_zero())) return 0;
	int n;
	is >> n;
	if (n<0) {
		delete bl;
		return 0;
	}
	for (int i=0; i<n; ++i) {
		int appid;
		is >> appid;

		delta* ag=delta::create(appid,is);
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


	//b::from_stream(is);
	//return bl;
}


bool c::allow(const miner_gut& g) { 
	lock_guard<mutex> lock(mx_proof_of_work);
	const auto& pubkeyh=g.pubkey.hash();
	auto i=proof_of_work.find(pubkeyh);
	if (i!=proof_of_work.end()) {
		return false;
	}
	proof_of_work.emplace(pubkeyh,0);
}

uint64_t c::add(int appid, local_delta* g) { //private, not protected by mutex, see add fn below
	auto i=find(appid);
	if (i==end()) {
		i=emplace(appid,delta::create(appid)).first;
	}
	return i->second->merge(g);
}

void c::add(miner_gut* mg) {
	uint64_t work=0;
	{
	lock_guard<mutex> lock(mx);
	//mg.create_consensuated_app_gut(*this);
	for (auto& i:*mg) {
		work+=add(i.first,i.second);
		i.second=0;
	}
	}

	{
	lock_guard<mutex> lock(mx_proof_of_work);
	proof_of_work.find(mg->pubkey.hash())->second=work;
	}

	delete mg;

	//auto i=emplace(pubk,g);
	//return i.second;
}

void c::end_adding() {
	lock_guard<mutex> lock(mx);
	for (auto& i:*this) {
		i.second->end_merge();
	}
	h=false;
}

/*
void block::hash(unsigned char hash[crypto::double_hash256::OUTPUT_SIZE]) const {
	ostringstream os;
	to_stream(os);
	cout << "Computing hash of block content:" << endl;
	cout << ">" << os.str() << "<" << endl;
	crypto::double_hash256 h;
	h.write(os.str());
	h.finalize(hash);
}
*/
#include <gov/crypto/base58.h>

const c::hash_t& c::hash() const {
//h=false;
	if (!h) {
		hasher_t hasher;
		ostringstream os;
		to_stream(os);
		hasher.write(os.str());
		hasher.finalize(hash_cached);
		h=true;
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

/*
//hash_cached.clear(); //TODO remove

	if (!h) {
		unsigned char h[crypto::double_hash256::OUTPUT_SIZE];
		hash(h);
		h=crypto::b58::encode(h,h+crypto::double_hash256::OUTPUT_SIZE);
	}
	cout << "hash_cached=" << hash_cached << endl;
	return hash_cached;
}

string block::hash(const string& content) {
	unsigned char hash[crypto::double_hash256::OUTPUT_SIZE];
	crypto::double_hash256 h;
	h.write(content);
	h.finalize(hash);
	return crypto::b58::encode(hash,hash+crypto::double_hash256::OUTPUT_SIZE);
}

*/


