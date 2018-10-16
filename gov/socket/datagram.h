#ifndef USGOV_39bdfeac516fc1718aed51b86fa8d93d0d07d38ad969652da578521c0fd96067
#define USGOV_39bdfeac516fc1718aed51b86fa8d93d0d07d38ad969652da578521c0fd96067

#include <stddef.h>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include "us/gov/crypto/ripemd160.h"

namespace us { namespace gov { namespace socket {
using namespace std;

class datagram: public vector<uint8_t> {

public:

    typedef crypto::ripemd160 hasher_t;
    typedef hasher_t::value_type hash_t;

    static constexpr size_t h{6};
    static constexpr size_t maxsize{100000};

    datagram();
    datagram(uint16_t service);
    datagram(uint16_t service, uint16_t);
    datagram(uint16_t service, const string&);

    bool completed() const;
    hash_t compute_hash() const;
    hash_t compute_payload_hash() const;

    string sendto(int sock) const;
    string recvfrom(int sock);

    string parse_string() const;
    uint16_t parse_uint16() const;

    void dump(ostream&) const;

    uint16_t service;

private:

    size_t m_dend;
    static string service_str(uint16_t svc);
    inline string service_str() const { return service_str(service); }

    uint32_t decode_size() const;
    uint16_t decode_service() const;
    void encode_size(uint32_t);
    void encode_service(uint16_t);
};

}}}

#endif

