#ifndef USGOV_6c9e30b55f8a9529f6284516edcde739a1fb239a4ec9403f8f7614eb8da58f0d
#define USGOV_6c9e30b55f8a9529f6284516edcde739a1fb239a4ec9403f8f7614eb8da58f0d

#include <string>
#include <array>

namespace us{ namespace gov{ namespace engine{
using namespace std;
class param_t {
public:
   enum consensus_t { majority=0, 
                      average, 
                      median, 
                      num_types };
    param_t(int id, consensus_t type): m_id(m_id), m_consensus_type(type) {}

    inline bool operator <(const param_t& other) const { return m_id < other.m_id; }

private:
    constexpr static array<const char*,num_types> m_typestr={"majority","average","median"};
    int m_id;
    consensus_t m_consensus_type;
};
}}}
#endif

