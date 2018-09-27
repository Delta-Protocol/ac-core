#include "syncd.h"
#include "daemon.h"

using namespace us::gov::engine;
using namespace std;

typedef us::gov::engine::syncd_t c;

c::syncd_t(daemon* d): d(d), head(0),cur(0),tail(0) {
}

const c::hash_t& c::tip() const {
    lock_guard<mutex> lock(mx);
    return head;
}

void c::run() {
    static constexpr int reset_countdown=100;
    int query_reps{reset_countdown};
    hash_t last_queryed;
	while(!program::_this.terminated) {
		cout << "SYNCD: Start" << endl;
		hash_t he(0), ta(0), cu(0);
		{
			vector<hash_t> patches;
			while(true) {
				//cout << "SYNCD: begin iteration " << endl;
				{
				lock_guard<mutex> lock(mx);
				if (cur==tail) break;
				if (head.is_zero()) break;
				if (cur.is_zero()) break;
				he=head;
				ta=tail;
				cu=cur;
				}
				if (cu==he) patches.clear();
//				cout << "SYNCD: head:" << he << " cur: " << cu << " tail: " << ta << " ;" << patches.size() << " patches" << endl;

				hash_t prev;
				auto filename=d->dfs().load(cu.to_b58());
				if (likely(d->get_prev(filename,prev))) { //file exists, read it
//					cout << "SYNCD: found file for " << cu << endl;
					patches.push_back(cu);
					lock_guard<mutex> lock(mx);
					cur=prev;
				}
				else {
					//the petition was delivered
                    if (last_queryed!=cu) {
                        last_queryed=cu;
                        query_reps=100;
                    }
                    else {
                        cout << "countdown to clearing " << query_reps << endl;
                        if (--query_reps==0) { // TODO we are in a deadlock
                            {
                                query_reps=100;
                        lock_guard<mutex> lock(mx);
                            head=cur=tail=0;
                            }
                            cout << "triggering db reset" << endl;
                            d->clear();
                        }
                    }
/*
                    {
                       unique_lock<mutex> lock(mx); //_wait4file);
                       if (!file_arrived) cv.wait_for(lock, 3s, [&]{ return file_arrived || program::_this.terminated; });
                       file_arrived=false;
                    }
                    */
				}
				if (program::_this.terminated) return;
			}
			if (!patches.empty()) {
				if (d->patch_db(patches)) {
					lock_guard<mutex> lock(mx);
					tail=*patches.begin();
				}
                else {
                    d->clear();
                }
			}
            d->on_sync();
		}
		wait();
		cout << "SYNCD: sync iteration completed" << endl;
	}
}
/*
void c::signal_file_arrived() {
       file_arrived=true;
       cv_wait4file.notify_all();
}
*/
void c::update(const diff::hash_t& h, const diff::hash_t& t) {
//cout << "syncd: UPDATE head " << h << " tail " << t << endl;
	{
	lock_guard<mutex> lock(mx);
	cur=head=h;
	tail=t;
	resume=true;
	}
	update();
}
void c::update(const diff::hash_t& t) {
//cout << "syncd: UPDATE tail " << t << endl;
	{
	lock_guard<mutex> lock(mx);
	tail=t;
	cur=head;
	resume=true;
	}
	update();
}
void c::update() {
//	cout << "SYNCD: received wakeup signal " << endl;
	cv.notify_all();
}
void c::wait() {
	unique_lock<mutex> lock(mx);
	cv.wait(lock, [&]{ return resume || program::_this.terminated; });
	resume=false;
}
void c::wait(const chrono::steady_clock::duration& d) {
	unique_lock<mutex> lock(mx);
	cv.wait_for(lock, d, [&]{ return resume || program::_this.terminated; });
	resume=false;
}
void c::on_finish() {
	cv.notify_all();
}

bool c::in_sync() const {
	lock_guard<mutex> lock(mx);
	return head==tail;
}

void c::dump(ostream& os) const {
	os << "Greetings from syncd" << endl;
	os << "in sync: " << boolalpha << in_sync() << endl;
	lock_guard<mutex> lock(mx);
	os << "head: " << head << endl;
	os << "cur: " << cur << endl;
	os << "tail: " << tail << endl;
}

