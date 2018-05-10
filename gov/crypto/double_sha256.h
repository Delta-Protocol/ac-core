#ifndef USGOV_7f56782e8ba6adc1469f36dc752c8f1d9729121ed3ab14b7f4b336757ba0af2c
#define USGOV_7f56782e8ba6adc1469f36dc752c8f1d9729121ed3ab14b7f4b336757ba0af2c

#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <cassert>
#include <cstring>
#include <gov/likely.h>
#include "sha256.h"
#include "ripemd160.h"

namespace us { namespace gov {
namespace crypto {
using namespace std;

	class double_sha256 {
	private:
	    sha256 sha;
	public:
	    typedef sha256::value_type value_type;
	    static const size_t output_size = sha256::output_size;

	    void finalize(unsigned char hash[output_size]);
	    void finalize(value_type& hash);
	    void write(const unsigned char *data, size_t len);
	    void write(const string&data);
	    void write(const value_type& data);
	    void write(const ripemd160::value_type& data);
	    void reset();

	};

}
}}

#endif

