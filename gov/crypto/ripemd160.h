#ifndef US_GOV_CRYPTO_RIPEMD160_H
#define US_GOV_CRYPTO_RIPEMD160_H

#include <stdint.h>
#include <stdlib.h>
#include <cassert>
#include <cstring>

#include <string>
#include <array>
#include <iostream>

namespace us{ namespace gov{ namespace crypto{
using namespace std;

class ripemd160	{
public:
    static constexpr size_t output_size = 20;

    class value_type: public array<unsigned char,output_size> {
    public: 
        value_type();
        value_type(unsigned int i);
        bool operator == (const value_type& other) const;
        bool operator != (const value_type& other) const;
        bool operator < (const value_type& other)  const; 
        void set(unsigned int i);

        value_type& operator = (const string& b58){
            set_b58(b58);
            return *this;
        }

        value_type& operator = (unsigned int i)   {
            set(i);
            return *this;
        }

        bool is_zero() const;
        void zero();

        string to_b58() const;
        string to_hex() const;

        static value_type from_b58(const string&);
        bool set_b58(const string&);
        static value_type from_hex(const string&);
    };

    ripemd160();

public:
    void finalize(unsigned char hash[output_size]);
    void finalize(value_type&);

public:
    void write(const unsigned char* data, size_t len);
    void write(const string&);
    void write(const value_type&);
    void write(const uint64_t&);
    void write(const int64_t&);
    void write(const uint32_t&);
    void write(const int32_t&);
    void write(bool);
    void write(const double&);
    void reset();

private:
    uint32_t s[5];
    unsigned char buf[64];
    uint64_t bytes;
    static const unsigned char pad[64];
};

inline ostream& operator << (ostream& os, const ripemd160::value_type& v) {
    os << v.to_b58();
	return os;
}

template<typename T>
inline ripemd160& operator << (ripemd160& h, const T& v) {
    h.write(v);
    return h;
}

inline istream& operator >> (istream& is, ripemd160::value_type& v) {
    string s;
    is >> s;
    if (!v.set_b58(s)) is.setstate(ios_base::failbit);
        return is;
}

}}}

namespace std {
template <>
struct hash<us::gov::crypto::ripemd160::value_type> {
    size_t operator()(const us::gov::crypto::ripemd160::value_type& k) const {
        return *reinterpret_cast<const size_t*>(&k[0]);
    }
};
}

#endif

