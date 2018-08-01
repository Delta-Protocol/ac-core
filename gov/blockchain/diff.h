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

	struct diff;

	struct local_deltas: map<int,app::local_delta*>, signed_data { /// indexed by app id;
		virtual ~local_deltas() {}
		string message_to_sign() const override;
		void to_stream(ostream&) const;
		static local_deltas* from_stream_prev(istream&);
		static local_deltas* from_stream(istream&);
	};

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
		typedef app::hasher_t hasher_t;
		typedef app::hash_t hash_t;

		diff() {}
		diff(const diff&)=delete;

		virtual ~diff() {
			for (auto& i:*this) delete i.second;
		}

		bool allow(const local_deltas&); // only one local_deltas per pubkey is allowed to be added
		uint64_t add(int appid, app::local_delta* g);
		void add(local_deltas*); //returns false when local_deltas exists for this pubk
		void end_adding();

		static hash_t hash(const string&);
		const hash_t& hash() const;

		void to_stream(ostream&) const;
		static diff* from_stream_prev(istream&);
		static diff* from_stream(istream&);


		mutable hash_t hash_cached;
		mutable bool h{false};
		hash_t prev; /// pointer to previous block
		pair<hash_t,hash_t> base; /// (5 cycles ago block hash, base hash) (5 diff ago can be applied to this base)

		mutex mx;

		pow_t proof_of_work;
		mutex mx_proof_of_work;
	};

}}
}

#endif
