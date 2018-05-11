#ifndef USGOV_5be98661b464a5d857a59dc71f31d27ce8d4c8f8f678e03cca8653a2a3372b9f
#define USGOV_5be98661b464a5d857a59dc71f31d27ce8d4c8f8f678e03cca8653a2a3372b9f

#include <vector>
#include <cassert>
#include <unordered_map>
#include <thread>
#include <us/gov/socket.h>
#include <us/gov/auth.h>
#include <map>
#include <us/gov/crypto/hash.h>
#include <us/gov/crypto/ec.h>
#include "app.h"
#include "signed_data.h"

namespace us { namespace gov {
namespace blockchain {
	using namespace std;
	using socket::datagram;
	struct peer_t;
	using crypto::ripemd160;

	typedef crypto::ec::keys keys;
	typedef keys::pub_t pubkey_t;
	typedef pubkey_t::hash_t pubkeyh_t;


//	struct signature:string {
//	};

	//struct local_delta;
	struct diff;
/*
	struct appguts_by_pubkey: unordered_map<string,app_gut*> {
		typedef unordered_map<string,app_gut*> b;
		appguts_by_pubkey() {}
		appguts_by_pubkey(istream&);
		appguts_by_pubkey(appguts_by_pubkey&& other): b(other) {}
		~appguts_by_pubkey();
		void to_stream(ostream&) const;

		app_gut* create_consensuated_app_gut(int appid) const;

	};
*/

	struct local_deltas: map<int,app::local_delta*>, signed_data { /// indexed by app id;
		virtual ~local_deltas() {}
		//typedef appguts_by_pubkey bucket;
		//local_deltas(const string& pubkey): pubkey(pubkey) {}

		//const bucket& get_app_guts(int id) const;

		//void create_consensuated_app_gut(map<int,app_gut*>&) const;

		//void purge(app_gut*) {} //remove empty stuff;
	

		//bool verify() const;

		string message_to_sign() const override;
		void to_stream(ostream&) const;
		static local_deltas* from_stream(istream&);
//		static bucket empty_bucket;
	};
/*
	struct app_guts:map<int,app_gut*> { /// indexed by app id; needs to be sorted, otherwise serialization will be undeterministic and block hash would fail
		//local_deltas(const string& pubkey): pubkey(pubkey) {}
		virtual ~app_guts();
	};
*/
	
	struct pow_t:map<pubkey_t::hash_t, uint64_t> {
		uint64_t sum() const {
			uint64_t s=0;
			for (auto&i:*this) {
				s+=i.second;
			}
			return s;
		}
	};

	struct diff: map<int,app::delta*> {
		typedef map<int,app::delta*> b;
//		typedef crypto::sha256 hasher_t;
//		typedef crypto::double_sha256 hasher_t;
		typedef app::hasher_t hasher_t;
		typedef app::hash_t hash_t;

		diff() {
			//assert(closure==0);
		}
		diff(const diff&)=delete;
			//assert(closure==0);
		
		virtual ~diff() {
			for (auto& i:*this) delete i.second;
			//delete closure; 
		}


//		void add(int appid, const app_gut& g);
		uint64_t add(int appid, app::local_delta* g);
		void add(local_deltas*); //returns false if already a local_deltas exists for this pubk
		void end_adding(); 

		static hash_t hash(const string&);
//		void hash(unsigned char hash[crypto::double_hash256::OUTPUT_SIZE]) const;
		const hash_t& hash() const;

		void to_stream(ostream&) const;
		static diff* from_stream(istream&);

		bool allow(const local_deltas& g);

		
		mutable hash_t hash_cached;
		mutable bool h{false};
		//local_deltas* closure{0};
		//bool checkpoint{false};
		hash_t prev; /// pointer to previous block
		pair<hash_t,hash_t> base; /// (5 cycles ago block hash, base hash) (5 diff ago can be applied to this base)
		
		mutex mx;

		pow_t proof_of_work;
		mutex mx_proof_of_work;

	};

}}
}

#endif

