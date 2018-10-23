#ifndef USGOV_8aa9f31d37f503d177a79739f204352b88f353e78c88f6d62fdcbe86273e7754
#define USGOV_8aa9f31d37f503d177a79739f204352b88f353e78c88f6d62fdcbe86273e7754

#include <chrono>
#include <string>
#include <atomic>
#include "diff.h"

namespace us{ namespace gov{ namespace engine{
using namespace std;
using namespace std;

class cycle_t {
public:
    typedef chrono::steady_clock::duration duration;
    typedef chrono::steady_clock::time_point time_point;
    typedef chrono::minutes minutes;
    typedef chrono::seconds seconds;
    enum stage {
        new_cycle=0,
        local_deltas_io=10,
        sync_db=20,
        consensus_vote_tip_io=40,
        num_stages=4
    };
		
    cycle_t():m_new_diff(0) {}
    ~cycle_t() {
        delete m_new_diff;
    }
    

public:
    void wait_for_stage(stage ts);

    diff* get_new_diff() const {
        return m_new_diff;
    }

    void set_new_diff(diff* d){
        m_new_diff = d;
    } 

private:
    string str(stage s) const;
    stage get_stage();

private:
    atomic<bool> m_in_sync;
    time_point m_cur_sync;
    duration m_period{60s};
    diff* m_new_diff;
};

}}}

#endif



