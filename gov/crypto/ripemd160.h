#ifndef USGOV_5a3346e45d07feaf4864a3e08a47717c8da7429ee7accc342f7bc496c9fdb846
#define USGOV_5a3346e45d07feaf4864a3e08a47717c8da7429ee7accc342f7bc496c9fdb846

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <cassert>
#include <cstring>
#include <array>
#include <iostream>

namespace us { namespace gov {
namespace crypto {
using namespace std;

	class ripemd160	{
	public:
	    static constexpr size_t output_size = 20;

	    struct value_type:array<unsigned char,output_size> {
		value_type();
		value_type(unsigned int i);
		bool operator == (const value_type& other) const;  //result depends on endianness (different results in different archs),ok for local hash tables
		bool operator != (const value_type& other) const;  //result depends on endianness (different results in different archs),ok for local hash tables
		bool operator < (const value_type& other) const; //result depends on endianness (different results in different archs),ok for local hash tables
		bool is_zero() const;
		void zero();

		string to_b58() const;
		string to_hex() const;
		static value_type from_b58(const string&);
		bool set_b58(const string&);
		static value_type from_hex(const string&);

	    };

	    ripemd160();
	    void write(const unsigned char* data, size_t len);
	    void write(const value_type&);
	    void write(const string&);
	    void write(const uint64_t&);
	    void write(const int64_t&);
	    void write(bool);
	    void write(const double&);

	    void finalize(unsigned char hash[output_size]);
	    void finalize(value_type&);
	    void reset();
	private:
	    uint32_t s[5];
	    unsigned char buf[64];
	    uint64_t bytes;
	    static const unsigned char pad[64];

	};


  inline ostream& operator << (ostream& os, const us::gov::crypto::ripemd160::value_type& v) {
	os << v.to_b58();
	return os;
  }

  template<typename T>
  inline us::gov::crypto::ripemd160& operator << (us::gov::crypto::ripemd160& h, const T& v) {
	h.write(v);
	return h;
  }

  inline istream& operator >> (istream& is, us::gov::crypto::ripemd160::value_type& v) {
	string s;
	is >> s;
	if (!v.set_b58(s)) is.setstate(ios_base::failbit);
	return is;
  }

}
}}

namespace std {

  template <>
  struct hash<us::gov::crypto::ripemd160::value_type> {
	size_t operator()(const us::gov::crypto::ripemd160::value_type& k) const {
		return *reinterpret_cast<const size_t*>(&k[0]);

	}
  };

}

#endif

