#include "double_sha256.h"

using namespace us::gov::crypto;

void double_sha256::finalize(unsigned char hash[sha256::output_size]) {
    unsigned char buf[m_sha.output_size];
    m_sha.finalize(buf);
    m_sha.reset();
    m_sha.write(buf, m_sha.output_size);
    m_sha.finalize(hash);
}

void double_sha256::finalize(sha256::value_type& hash) {
    unsigned char buf[m_sha.output_size];
    m_sha.finalize(buf);
    m_sha.reset();
    m_sha.write(buf, m_sha.output_size);
    m_sha.finalize(hash);
}

void double_sha256::write(const unsigned char *data, size_t len) {
    if(likely(len>0)) 
        m_sha.write(data, len);
}

void double_sha256::write(const std::string& data) {
    if(unlikely(data.empty())) 
        return;
    m_sha.write(reinterpret_cast<const unsigned char*>(&data[0]), data.size());
}

void double_sha256::write(const sha256::value_type& data) {
    write(&data[0],sha256::output_size);
}

void double_sha256::write(const ripemd160::value_type& data) {
    write(&data[0],ripemd160::output_size);
}

void double_sha256::write(bool data) {
    write(reinterpret_cast<const unsigned char*>(&data),sizeof(data));
}

void double_sha256::reset() {
    m_sha.reset();
}


