#include "syncd.h"
#include "daemon.h"

using namespace us::gov::engine;
using namespace std;

syncd_t::syncd_t(daemon* d): m_d(d), m_head(0),m_cur(0),m_tail(0) {
}

const syncd_t::hash_t& syncd_t::tip() const {
    lock_guard<mutex> lock(m_mx);
    return m_head;
}

void syncd_t::run() {
    static constexpr int reset_countdown=100;
    int query_reps{reset_countdown};
    hash_t last_queryed;

    while(!program::_this.terminated) {
        cout << "SYNCD: Start" << endl;
        hash_t he(0), ta(0), cu(0);
	{
            vector<hash_t> patches;
            while(true) {
                {
                    lock_guard<mutex> lock(m_mx);
                    if (m_cur==m_tail) 
                        break;
                    if (m_head.is_zero()) 
                        break;
                    if (m_cur.is_zero()) 
                        break;
                    he=m_head;
                    ta=m_tail;
                    cu=m_cur;
                }
                if (cu==he) 
                    patches.clear();
                hash_t prev;
                auto filename=m_d->dfs().load(cu.to_b58());
                if (likely(m_d->get_prev(filename,prev))) { 
                    patches.push_back(cu);
                    lock_guard<mutex> lock(m_mx);
                    m_cur=prev;
                }else {
                    if (last_queryed!=cu) {
                        last_queryed=cu;
                        query_reps=100;
                    }else {
                        cout << "countdown to clearing " << query_reps << endl;
                        if (--query_reps==0) {
                            {
                                query_reps=100;
                                lock_guard<mutex> lock(m_mx);
                                m_head=m_cur=m_tail=0;
                            }
                            cout << "triggering db reset" << endl;
                            m_d->clear();
                        }
                    }
                }
                if (program::_this.terminated) 
                    return;
            }   
            if (!patches.empty()) {
                if (m_d->patch_db(patches)) {
                    lock_guard<mutex> lock(m_mx);
                    m_tail=*patches.begin();
                }else {
                    m_d->clear();
                }
            }
            m_d->on_sync();
        }
        wait();
        cout << "SYNCD: sync iteration completed" << endl;
    }
}

void syncd_t::update(const diff::hash_t& h, const diff::hash_t& t) {
    {
        lock_guard<mutex> lock(m_mx);
        m_cur=m_head=h;
        m_tail=t;
        resume=true;
    }
    update();
}

void syncd_t::update(const diff::hash_t& t) {
    {
        lock_guard<mutex> lock(m_mx);
        m_tail=t;
        m_cur=m_head;
        resume=true;
    }
    update();
}

void syncd_t::update() {
    m_cv.notify_all();
}

void syncd_t::wait() {
    unique_lock<mutex> lock(m_mx);
    m_cv.wait(lock, [&]{ return resume || program::_this.terminated; });
    resume=false;
}

void syncd_t::wait(const chrono::steady_clock::duration& d) {
    unique_lock<mutex> lock(m_mx);
    m_cv.wait_for(lock, d, [&]{ return resume || program::_this.terminated; });
    resume=false;
}

void syncd_t::on_finish() {
    m_cv.notify_all();
}

bool syncd_t::in_sync() const {
    lock_guard<mutex> lock(m_mx);
    return m_head==m_tail;
}

void syncd_t::dump(ostream& os) const {
    os << "Greetings from syncd" << endl;
    os << "in sync: " << boolalpha << in_sync() << endl;
    lock_guard<mutex> lock(m_mx);
    os << "head: " << m_head << endl;
    os << "cur: "  << m_cur << endl;
    os << "tail: " << m_tail << endl;
}

