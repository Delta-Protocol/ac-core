#ifndef USGOV_075dfff39e95d4a4a7a088fe2ea0fc252d6fa47e6b62b043b4babd2e2b511780
#define USGOV_075dfff39e95d4a4a7a088fe2ea0fc252d6fa47e6b62b043b4babd2e2b511780

#include <secp256k1.h>
#include <iostream>
#include <vector>
#include "double_sha256.h"

namespace us { namespace gov {
namespace crypto {
using namespace std;

struct ec {
	typedef secp256k1_ecdsa_signature signature;
	typedef double_sha256 sigmsg_hasher_t;

	ec();
	~ec();

	struct keys {
		struct pub_t:secp256k1_pubkey { //64 bytes length
			typedef crypto::ripemd160 hasher_t;
			typedef hasher_t::value_type hash_t;

			inline const hash_t& hash() const { 
				if (!h) {
					hash_cached=compute_hash();
					h=true;
				}
				return hash_cached;
			}
			hash_t compute_hash() const;
			string to_b58() const;
			string to_hex() const;
			bool set_b58(const string&);
			static pub_t from_b58(const string&);
			static pub_t from_hex(const string&);
			void assign(const string&);

			bool operator == (const pub_t& other) const;
			pub_t& operator = (const pub_t& other);

		private:
			mutable bool h{false};
			mutable hash_t hash_cached;
		};

		struct priv_t: array<unsigned char,32> {
			string to_b58() const;
			static priv_t from_b58(const string&);
			bool set_b58(const string&);
		};

		keys() {}
		keys(const priv_t& pk);
		keys(const string& privk_b58);
		keys(const keys& other);
		keys(keys&& other);

		priv_t priv;
		pub_t pub;

		void dump(ostream& os) const;
		static string to_hex(const priv_t& privkey);

		static keys generate();

		static bool verify(const priv_t& privkey);
		static pub_t get_pubkey(const priv_t& privkey);

	};

	vector<unsigned char> sign(const keys::priv_t& pk, const string& text) const;
	vector<unsigned char> sign(const keys::priv_t& pk, const sigmsg_hasher_t::value_type&) const;
	string sign_encode(const keys::priv_t& pk, const string& text) const;
	string sign_encode(const keys::priv_t& pk, const sigmsg_hasher_t::value_type&) const;

	bool verify(const keys::pub_t& pk, const string& text, const string& signature_der) const;
	bool verify(const keys::pub_t& pk, const sigmsg_hasher_t::value_type& msgh, const string& signature_der_b58) const;

	static constexpr size_t bit_size() { return 256; }

	static string to_hex(const vector<unsigned char>& data);
	static string to_b58(const vector<unsigned char>& data);
	static vector<unsigned char> from_hex(const string& hex);
	static vector<unsigned char> from_b58(const string& hex);

	secp256k1_context *ctx;
	static ec instance; 
};

inline ostream& operator << (ostream& os, const ec::keys::pub_t& k) {
	os << k.to_b58();
	return os;
}

inline istream& operator >> (istream& is, ec::keys::pub_t& k) {
	string s;
	is >> s;
	k.set_b58(s);
	return is;
}

inline ostream& operator << (ostream& os, const ec::keys::priv_t& k) {
	os << k.to_b58();
	return os;
}

inline istream& operator >> (istream& is, ec::keys::priv_t& k) {
	string s;
	is >> s;
	k.set_b58(s);
	return is;
}

}
}}

#endif
