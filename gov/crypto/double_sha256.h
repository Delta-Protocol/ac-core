#ifndef USGOV_7f56782e8ba6adc1469f36dc752c8f1d9729121ed3ab14b7f4b336757ba0af2c
#define USGOV_7f56782e8ba6adc1469f36dc752c8f1d9729121ed3ab14b7f4b336757ba0af2c

#include <string>
#include <us/gov/likely.h>
#include "sha256.h"
#include "ripemd160.h"

namespace us{ namespace gov{ namespace crypto{

class double_sha256 {
private:
    void finalize(unsigned char hash[sha256::output_size]);
    void finalize(sha256::value_type& hash);
    void write(const unsigned char *data, size_t len);
    void write(const std::string& data);
    void write(const sha256::value_type& data);
    void write(const ripemd160::value_type& data);
    void write(bool);
    void reset();
    sha256 m_sha;
};

}}}

#endif

