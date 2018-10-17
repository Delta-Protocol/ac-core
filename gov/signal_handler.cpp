#include "signal_handler.h"
#include <iostream>
#include "unistd.h"

using namespace us::gov;
using namespace std;

signal_handler signal_handler::_this;

#ifdef SIM

#else

void signal_handler::sleep_for(const chrono::steady_clock::duration& d) {
    unique_lock<mutex> lock(m_mx);
    if (terminated) return;
    m_cv.wait_for(lock, d, [&]{ return terminated; });
}

void signal_handler::sleep_until(const chrono::steady_clock::time_point& d) {
    unique_lock<mutex> lock(m_mx);
    if (terminated) return;
    m_cv.wait_until(lock, d, [&]{ return terminated; });
}

#endif

void signal_handler::finish() {
    if (terminated) return;
    terminated=true;
    m_cv.notify_all(); //wakeup speeping threads
    for (auto&i:m_callbacks) i->on_finish(); //wake up socket listener
}

