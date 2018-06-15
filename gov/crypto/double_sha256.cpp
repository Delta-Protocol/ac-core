#include "double_sha256.h"
#include "endian_rw.h"
#include "base58.h"
#include <us/gov/likely.h>
#include <cstring>

using namespace us::gov::crypto;
using namespace std;

typedef us::gov::crypto::double_sha256 c;

void c::finalize(unsigned char hash[output_size]) {
	unsigned char buf[sha.output_size];
	sha.finalize(buf);
	sha.reset();
	sha.write(buf, sha.output_size);
	sha.finalize(hash);
}

void c::finalize(value_type& hash) {
	unsigned char buf[sha.output_size];
	sha.finalize(buf);
	sha.reset();
	sha.write(buf, sha.output_size);
	sha.finalize(hash);
}

void c::write(const unsigned char *data, size_t len) {
	if(likely(len>0)) sha.write(data, len);
}

void c::write(const string&data) {
	if(unlikely(data.empty())) return;
	sha.write(reinterpret_cast<const unsigned char*>(&data[0]), data.size());
}

void c::write(const value_type& data) {
	write(&data[0],output_size);
}

void c::write(const ripemd160::value_type& data) {
	write(&data[0],ripemd160::output_size);
}

void c::write(bool data) {
	write(reinterpret_cast<const unsigned char*>(&data),sizeof(data));
}


void c::reset() {
	sha.reset();
}


