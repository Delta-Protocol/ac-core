#ifndef USGOV_c7f3786a215777af6d278776cd1585700df3c118631b0669869d84d1140ea704
#define USGOV_c7f3786a215777af6d278776cd1585700df3c118631b0669869d84d1140ea704

#include <condition_variable>
#include <mutex>
#include <chrono>
#include <unordered_set>
#include <mutex>
#include <map>

namespace us { namespace gov {
using namespace std;

struct signal_handler {
	struct callback {
		virtual void on_finish()=0;
	};

	void sleep_for(const chrono::steady_clock::duration&);
	void sleep_until(const chrono::system_clock::time_point&);

	void finish();

	volatile bool terminated{false};

	inline void add(callback*i) { callbacks.emplace(i); }
	inline void remove(callback*i) { callbacks.erase(callbacks.find(i)); }

	static signal_handler _this;

private:
	unordered_set<callback*> callbacks;
	condition_variable cv;
	mutex mx;
};

typedef signal_handler thread_;
typedef signal_handler program;

}}


#endif
