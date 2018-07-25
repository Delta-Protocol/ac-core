
#ifndef USGOV_8aa9f31d37f503d177a79739f204352b88f353e78c88f6d62fdcbe86273e7754
#define USGOV_8aa9f31d37f503d177a79739f204352b88f353e78c88f6d62fdcbe86273e7754

#include <chrono>
#include <string>
#include <atomic>
#include "diff.h"

namespace us{ namespace gov {
namespace blockchain {
using namespace std;
    using namespace std;


	struct cycle_t {
		typedef chrono::system_clock::duration duration;
		typedef chrono::system_clock::time_point time_point;
		typedef chrono::minutes minutes;
		typedef chrono::seconds seconds;
		enum stage {
			new_cycle=0,
			local_deltas_io=10,
			sync_db=20,
			consensus_vote_tip_io=40,
            num_stages=4
		};
		
    	cycle_t():new_diff(0) {
        }
    	~cycle_t() {
            delete new_diff;
        }
    
        atomic<bool> in_sync;

		void wait_for_stage(stage ts);
		string str(stage s) const;
		stage get_stage();

		time_point cur_sync;
		duration period{60s};
		diff* new_diff;
	};


}}
}

#endif



