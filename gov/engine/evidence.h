#ifndef US_GOV_ENGINE_EVIDENCE_H
#define US_GOV_ENGINE_EVIDENCE_H

#include <chrono>

namespace us{ namespace gov{ namespace engine{
using namespace std;
using namespace chrono;

class evidence {
public:
    evidence(): m_ts(duration_cast<nanoseconds>(system_clock::now().time_since_epoch()).count()) {}
    virtual ~evidence() {}

    uint64_t get_ts() const {
        return m_ts;
    }
   
    void set_ts(uint64_t ts){
        m_ts = ts;
    } 
private:
    uint64_t m_ts;
};

}}}

#endif



