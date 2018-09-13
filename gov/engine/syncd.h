#ifndef USGOV_77adc1ef338434dcbe2fd89f5a86d54ba6fe219dcb84f80d6903a1ee04fc73ea
#define USGOV_77adc1ef338434dcbe2fd89f5a86d54ba6fe219dcb84f80d6903a1ee04fc73ea

#include <condition_variable>
#include <mutex>
#include <chrono>
#include <us/gov/signal_handler.h>
#include "diff.h"


namespace us{ namespace gov {
namespace engine {
using namespace std;

		struct syncd_t: us::gov::signal_handler::callback {
			typedef diff::hash_t hash_t;
			syncd_t(daemon* d);
			void dump(ostream& os) const;

			void run();
			void update(const hash_t& head, const hash_t& tail);
			void update(const hash_t& tail);
			void update();
			void wait();
			void wait(const chrono::steady_clock::duration& d);
			virtual void on_finish();

            const hash_t& tip() const;

			daemon* d;
			condition_variable cv;
			mutable mutex mx;
			bool in_sync() const;
			hash_t head;
			hash_t cur;
			hash_t tail;
			bool resume{false};

			void signal_file_arrived();

		    
            condition_variable cv_wait4file;
		    mutex mx_wait4file;
			bool file_arrived{false};


		};

}}
}

#endif

