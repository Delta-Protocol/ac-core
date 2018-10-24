#ifndef US_GOV_ENGINE_POLICIES_H
#define US_GOV_ENGINE_POLICIES_H

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

