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
		datagram(uint16_t service, vector<uint8_t>&&);


		bool completed() const;
		hash_t compute_hash() const;
        static string service_str(uint16_t svc);
        inline string service_str() const { return service_str(service); }

		string sendto(int sock) const;
		string recvfrom(int sock);

		vector<string> parse_strings() const;
		string parse_string() const;
		uint16_t parse_uint16() const;

        void dump(ostream&) const;
	private:
		uint32_t decode_size() const;
		uint16_t decode_service() const;
		void encode_size(uint32_t);
		void encode_service(uint16_t);

	public: //TODO change to private
		uint16_t service;
		//size_t cur;
		size_t dend;
//		int error;
	};
}
}}

#endif

