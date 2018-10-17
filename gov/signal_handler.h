#ifndef USGOV_c7f3786a215777af6d278776cd1585700df3c118631b0669869d84d1140ea704
#define USGOV_c7f3786a215777af6d278776cd1585700df3c118631b0669869d84d1140ea704

#include <condition_variable>
#include <unordered_set>
#include <chrono>
#include <mutex>
#include <map>

namespace us { namespace gov {
using namespace std;

class signal_handler {

public:

    class callback {
    public:
        virtual void on_finish()=0;
    };

    void sleep_for(const chrono::steady_clock::duration&);
    void sleep_until(const chrono::steady_clock::time_point&);

    void finish();

    volatile bool terminated{false};

    inline void add(callback*i) { m_callbacks.emplace(i); }
    inline void remove(callback*i) { m_callbacks.erase(m_callbacks.find(i)); }

    static signal_handler _this;

private:

    unordered_set<callback*> m_callbacks;
    condition_variable m_cv;
    mutex m_mx;
};

typedef signal_handler thread_;
typedef signal_handler program;

}}

#endif
