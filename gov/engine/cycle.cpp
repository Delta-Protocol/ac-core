#include "cycle.h"

using namespace us::gov::engine;
using namespace std;
using namespace chrono;

void cycle_t::wait_for_stage(stage ts) {
    time_point now=steady_clock::now();
    duration tp = now.time_since_epoch();
    minutes m = duration_cast<minutes>(tp);
    tp-=m; 
    seconds s = duration_cast<seconds>(tp); 
    int n=ts;
    while (s.count()>n) 
        n+=60;
    cout << "Cycle: current second is " << s.count() 
         << ". I'll sleep for " << (n-s.count()) << endl;
    thread_::_this.sleep_for(seconds(n-s.count()));
    cout << "engine: starting stage: " << str(ts) << endl;
}

string cycle_t::str(stage s) const {
    switch(s) {
        case new_cycle: 
            return "new_cycle";
        case local_deltas_io: 
            return "local_deltas_io"; 
        case sync_db: 
            return "sync_db"; 
        case consensus_vote_tip_io: 
            return "consensus_vote_tip_io";
    }
    return "?";
}

cycle_t::stage cycle_t::get_stage() {
    time_point now=steady_clock::now();
    duration tp = now.time_since_epoch();
    minutes m = duration_cast<minutes>(tp);
    tp-=m;
    seconds s = duration_cast<seconds>(tp);
    int sec=s.count();
    if (sec<local_deltas_io) 
        return new_cycle;
    if (sec<consensus_vote_tip_io) 
        return local_deltas_io;
    if (sec<sync_db) return sync_db;
        return consensus_vote_tip_io;
}


