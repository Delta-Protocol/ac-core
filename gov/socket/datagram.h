#ifndef USGOV_39bdfeac516fc1718aed51b86fa8d93d0d07d38ad969652da578521c0fd96067
#define USGOV_39bdfeac516fc1718aed51b86fa8d93d0d07d38ad969652da578521c0fd96067

#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <csignal>
#include <cassert>
#include <memory>
#include <mutex>
#include <us/gov/net.h>
#include <us/gov/crypto/ripemd160.h>

namespace us { namespace gov {
namespace socket {
	using namespace std;
	struct datagram:vector<uint8_t> {
		typedef vector<uint8_t> b;

		typedef crypto::ripemd160 hasher_t;
		typedef hasher_t::value_type hash_t;

		static constexpr size_t h{6};
		static constexpr size_t maxsize{100000};
		datagram();
		datagram(uint16_t service);
		datagram(uint16_t service, uint16_t);
		datagram(uint16_t service, const string&);
		datagram(uint16_t service, vector<uint8_t>&&) {}

		bool completed() const;
		
		hash_t compute_hash() const;

//		void* wp();
//		uint32_t remaining() const;
		bool send(int sock) const;
		bool recv(int sock);
		bool recv(int sock, int timeout_seconds);

		vector<string> parse_strings() const;
		string parse_string() const;
		uint16_t parse_uint16() const;
	private:
		uint32_t decode_size() const;
		uint16_t decode_service() const;
		void encode_size(uint32_t);
		void encode_service(uint16_t);

	public: //TODO change to private
		uint16_t service;
		//size_t cur;
		size_t dend;
		int error;
	};
}
}}

#endif

