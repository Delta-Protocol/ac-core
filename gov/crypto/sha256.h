#ifndef US_GOV_CRYPTO_SHA256_H
#define US_GOV_CRYPTO_SHA256_H

#include <stdint.h>
#include <stdlib.h>
#include <cassert>
#include <cstring>

#include <string>
#include <array>

#include <us/gov/likely.h>
#include "ripemd160.h"

namespace us{ namespace gov{ namespace crypto{
using namespace std;

class sha256 {
public:
    static constexpr size_t output_size = 32;
    sha256();

    class value_type: public array<unsigned char,output_size> {
    public:
        value_type() {}
        value_type(int) { zero(); }

        bool operator < (const value_type& other) const; 
    
        bool is_zero() const;
        void zero();

        string to_b58() const;
        string to_hex() const;
        static value_type from_b58(const string&);
        static value_type from_hex(const string&);
    };

    void write(const unsigned char* data, size_t len);
    void write(const string&data);
    void write(const ripemd160::value_type& data);

    void finalize(unsigned char hash[output_size]);
    void finalize(value_type&);
    void reset();

private:
    void initialize(uint32_t* s);
   
    uint32_t inline Ch(uint32_t x, uint32_t y, uint32_t z) { 
        return z ^ (x & (y ^ z)); 
    }
    uint32_t inline Maj(uint32_t x, uint32_t y, uint32_t z) { 
        return (x & y) | (z & (x | y)); 
    }
   
    uint32_t inline Sigma0(uint32_t x) { 
        return (x >> 2 | x << 30) ^ (x >> 13 | x << 19) ^ (x >> 22 | x << 10); 
    }
    uint32_t inline Sigma1(uint32_t x) { 
        return (x >> 6 | x << 26) ^ (x >> 11 | x << 21) ^ (x >> 25 | x << 7); 
    }
    uint32_t inline sigma0(uint32_t x) { 
        return (x >> 7 | x << 25) ^ (x >> 18 | x << 14) ^ (x >> 3); 
    }
    uint32_t inline sigma1(uint32_t x) { 
        return (x >> 17 | x << 15) ^ (x >> 19 | x << 13) ^ (x >> 10); 
    }
   
    void inline Round(uint32_t a, uint32_t b, uint32_t c, uint32_t& d, 
                      uint32_t e, uint32_t f, uint32_t g, uint32_t& h, 
                      uint32_t k, uint32_t w)                        {
        uint32_t t1 = h + Sigma1(e) + Ch(e, f, g) + k + w;
        uint32_t t2 = Sigma0(a) + Maj(a, b, c);
        d += t1;
        h = t1 + t2;
    }
    void transform(uint32_t* s, const unsigned char* chunk);

private:
    uint32_t s[8];
    unsigned char buf[64];
    uint64_t bytes;
};

inline ostream& operator << (ostream& os, const sha256::value_type& v) {
    os << v.to_b58();
    return os;
}

inline istream& operator >> (istream& is, const sha256::value_type& v) {
    string s;
    is >> s;
    v.from_b58(s);
    return is;
}

}}}

namespace std {
template <>
struct hash<us::gov::crypto::sha256::value_type> {
    size_t operator()(const us::gov::crypto::sha256::value_type& k) const {
        return *reinterpret_cast<const size_t*>(&k[0]);
    }
};
}

#endif

