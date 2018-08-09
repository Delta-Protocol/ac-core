#ifndef USGOV_7e890654e98bf7c2878b37b6916dcb394f32fdcde3e1c8e0ac04dfd8d224f02c
#define USGOV_7e890654e98bf7c2878b37b6916dcb394f32fdcde3e1c8e0ac04dfd8d224f02c

#include <us/gov/crypto/hash.h>
#include <us/gov/crypto/ec.h>
#include <string>

namespace us { namespace gov {
namespace blockchain {
	using namespace std;
	using crypto::ripemd160;
	typedef crypto::ec::keys keys;
	typedef keys::pub_t pubkey_t;
	typedef pubkey_t::hash_t pubkeyh_t;

	//struct signature:string {
	//};

	struct signed_data {
		virtual ~signed_data() {}
		pubkey_t pubkey; 
		string signature; //der-b58
		virtual string message_to_sign() const=0;
		void sign(const crypto::ec::keys&);
		bool verify() const;
	};

}}
}

#endif

