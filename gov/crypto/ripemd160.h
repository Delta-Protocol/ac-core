#ifndef USGOV_5a3346e45d07feaf4864a3e08a47717c8da7429ee7accc342f7bc496c9fdb846
#define USGOV_5a3346e45d07feaf4864a3e08a47717c8da7429ee7accc342f7bc496c9fdb846

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <cassert>
#include <cstring>
#include <array>
#include <iostream>

namespace usgov {
namespace crypto {
using namespace std;

	class ripemd160	{
	public:
	    static constexpr size_t output_size = 20;

	    struct value_type:array<unsigned char,output_size> {
		value_type();
		value_type(unsigned int i);
		bool operator == (const value_type& other) const;  //result depends on endianness (different results in different archs),ok for local hash tables
		bool operator < (const value_type& other) const; //result depends on endianness (different results in different archs),ok for local hash tables
		bool is_zero() const;
		void zero();

		string to_b58() const;
		string to_hex() const;
		static value_type from_b58(const string&);
		void set_b58(const string&);
		static value_type from_hex(const string&);

	    };

	    ripemd160();
	    void write(const unsigned char* data, size_t len);
	    void write(const value_type& data);
	    void write(const string&data);
	    void write(const uint64_t&data);

	    void finalize(unsigned char hash[output_size]);
	    void finalize(value_type&);
	    void reset();
	private:
	    uint32_t s[5];
	    unsigned char buf[64];
	    uint64_t bytes;
	    static const unsigned char pad[64];

	};


  inline ostream& operator << (ostream& os, const usgov::crypto::ripemd160::value_type& v) {
	os << v.to_b58();
	return os;
  }

  template<typename T>
  inline usgov::crypto::ripemd160& operator << (usgov::crypto::ripemd160& h, const T& v) {
	h.write(v);
	return h;
  }

  inline istream& operator >> (istream& is, usgov::crypto::ripemd160::value_type& v) {
	string s;
	is >> s;
	v.set_b58(s);
	return is;
  }

}}

namespace std {

  template <>
  struct hash<usgov::crypto::ripemd160::value_type> {
	size_t operator()(const usgov::crypto::ripemd160::value_type& k) const {
		return *reinterpret_cast<const size_t*>(&k[0]);

	}
  };

}

#endif

