#include "signal_handler.h"
#include <iostream>
#include "unistd.h"

using namespace us::gov;
using namespace std;

typedef us::gov::signal_handler c;

c c::_this;

#ifdef SIM

#else

void c::sleep_for(const chrono::steady_clock::duration& d) {
	unique_lock<mutex> lock(mx);
	if (terminated) return;
	cv.wait_for(lock, d, [&]{ return terminated; });
}

void c::sleep_until(const chrono::system_clock::time_point& d) {
	unique_lock<mutex> lock(mx);
	if (terminated) return;
	cv.wait_until(lock, d, [&]{ return terminated; });
}

#endif


void c::finish() {
	if (terminated) return;
	cout << "starting ordered exit" << endl;
	terminated=true;
	cv.notify_all(); //wakeup speeping threads
	for (auto&i:callbacks) i->on_finish(); //wake up socket listener
}

