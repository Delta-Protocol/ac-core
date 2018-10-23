#ifndef USGOV_77adc1ef338434dcbe2fd89f5a86d54ba6fe219dcb84f80d6903a1ee04fc73ea
#define USGOV_77adc1ef338434dcbe2fd89f5a86d54ba6fe219dcb84f80d6903a1ee04fc73ea

#include <condition_variable>
#include <mutex>
#include <chrono>
#include <us/gov/signal_handler.h>
#include "diff.h"


namespace us{ namespace gov{ namespace engine{
using namespace std;

class syncd_t: public us::gov::signal_handler::callback {
public:
    typedef diff::hash_t hash_t;
    syncd_t(daemon* d);

    void dump(ostream& os) const;
    void run();

    void update(const hash_t& head, const hash_t& tail);
    void update(const hash_t& tail);

    bool in_sync() const;

private:
    void update();

    void wait();
    void wait(const chrono::steady_clock::duration& d);

    virtual void on_finish();

    const hash_t& tip() const;


    bool resume{false};
    bool file_arrived{false};

private:
    daemon* m_d;
    condition_variable m_cv;
    mutable mutex m_mx;

    hash_t m_head;
    hash_t m_cur;
    hash_t m_tail;
};

}}}

#endif

