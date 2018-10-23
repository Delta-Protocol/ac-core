#include "daemon.h"
#include <fstream>
#include <us/gov/auth.h>
#include <us/gov/likely.h>
#include "protocol.h"
#include <us/gov/signal_handler.h>
#include <us/gov/crypto/base58.h>

using namespace us::gov::engine;
using namespace std;

void daemon::constructor() {
    m_auth_app=new auth::app(m_id.get_pubkey());
    add(m_auth_app);
}

daemon::daemon(const keys& k, const string& home, 
               uint16_t port, uint8_t edges, 
               const vector<string>& seed_nodes): 
                    m_rng(chrono::steady_clock::now().time_since_epoch().count()), 
                    m_peerd(port, edges, this, seed_nodes, home), 
                    m_home(home), m_syncd(this), m_sysops(*this), m_id(k) {
    constructor();
}

daemon::~daemon() {
    delete m_pool;
}

void daemon::add(app*app) {
    if (m_apps_.find(app->get_id())!=m_apps_.end()) {
        cerr << "Fatal error: App collision. Two apps with same id." << endl;
        exit(1);
    }
    m_apps_.emplace(app->get_id(),app);
}


bool daemon::patch_db(const vector<diff::hash_t>& patches) {
    cout << "Applying " << patches.size() << " patches" << endl;
    for (auto i=patches.rbegin(); i!=patches.rend(); ++i) {
        const diff::hash_t& hash=*i;
        string filename=dfs().get_path_from(hash.to_b58());
        ifstream is(filename);
        if (!is.good()) {
            cerr << "corrupt filename 1 " << filename << endl;
            remove(filename.c_str());
            return false;
        }
        diff* b=diff::from_stream(is);
        if (!b) {
            cerr << "corrupt filename 2 " << filename << endl;
            remove(filename.c_str());
            return false;
        }
        if (b->hash()!=hash) {
            cerr << "corrupt filename 3 " << filename << endl;
            remove(filename.c_str());
            delete b;
            return false;
        }
        if (!import(*b)) {
            delete b;
            return false;
        }
        delete b;
    }

    diff::hash_t lbi;
    {
        lock_guard<mutex> lock(m_mx_import);
        lbi=m_last_block_imported;
    }
    m_syncd.update(lbi);
    return true;
}

const diff::hash_t& daemon::dbhash_off() const {
    if (!m_cached_dbhash_ok) {
        diff::hasher_t h;
        for (auto&i:m_apps_) {
            i.second->dbhash(h);
        }
        h.finalize(m_cached_dbhash);
        m_cached_dbhash_ok=true;
    }
    return m_cached_dbhash;
}

diff::hash_t daemon::get_last_block_imported() const {
    lock_guard<mutex> lock(m_mx_import);
    return m_last_block_imported;
}

void daemon::eat_diff(const diff::hash_t& voted_tip, 
                                      cycle_t& cycle) {
    cout << "eat_diff" << endl;
    if (likely(cycle.get_new_diff()!=nullptr)) {
        diff* nd = cycle.get_new_diff();
        if (likely(voted_tip==nd->hash())) {
            cout << "saving: " << nd->hash() << endl;
            dfs().save(nd->hash().to_b58(),nd->parse_string());
            assert(dfs().exists(nd->hash().to_b58()));
            if (!import(*cycle.get_new_diff())) {
                clear();
                cerr << "DB CLEARED" << endl;
            }
        }else {
            cout << "WARNING: tip is not the most voted one: " << nd->hash() << endl;
        }
        delete cycle.get_new_diff();
        cycle.set_new_diff(nullptr);
    } else {
        cout << "WARNING: block not computed " << endl;
    }
    m_syncd.update(voted_tip,get_last_block_imported());
}

void daemon::stage1(cycle_t& data) {
    if (data.get_new_diff()==nullptr) 
        return;
    if (m_auth_app->my_stage()!=peer_t::node) 
        return;
    vote_tip(*data.get_new_diff());
}

void daemon::stage2(cycle_t& cycle) {
    diff::hash_t tip=m_votes.select();
    app::get_chain_info().set_tip(tip); 

    cout << "Voting process result: diff " << tip << endl;
    if (likely(!tip.is_zero())) {
        eat_diff(tip,cycle);
    }
}

bool daemon::stage3(cycle_t& cycle) {
    if (unlikely(!m_auth_app->is_node())) 
        return false;
    if (unlikely(!m_syncd.in_sync())) 
        return false;

    auto* mg=create_local_deltas();
    if (mg!=0) {
        send(*mg);
        {
            lock_guard<mutex> lock(m_mx_pool);
            if (m_pool->allow(*mg)) 
                m_pool->add(mg);
        }
    }
    return true;
}

void daemon::stage4(cycle_t& cycle) {

    assert(cycle.get_new_diff()==0);
    {
        lock_guard<mutex> lock(m_mx_pool);
        m_pool->end_adding();
        cycle.set_new_diff(m_pool);
        m_pool=new diff();
    }
    cycle.get_new_diff()->set_previews_hash(get_last_block_imported());
}

void daemon::load_head() {
    string filename = m_home+"/head";
    ifstream is(filename);
    diff::hash_t head(0);
    if (is.good()) 
        is >> head;
    m_syncd.update(head,get_last_block_imported());
    app::get_chain_info().set_tip(head);
}

void daemon::evidence_processor() {
    static constexpr seconds idle_wakeup {1};
    static constexpr seconds relay_interval {5};
    uint64_t last_ev_ts=duration_cast<nanoseconds>((system_clock::now()-
                                     relay_interval).time_since_epoch()).count();
    int max=0;
    int rst=0;

    while(!program::_this.terminated) {
        evidence* e;
        app* p;
        {
            unique_lock<mutex> lock(m_calendar.mx);
            if (++rst>300) {
                max=0; 
                rst=0; 
            }
            if (m_calendar.size()>max) 
                max=m_calendar.size();        
            m_evidences_on_hold.wait_empty();

            auto i=m_calendar.begin();
            if (i==m_calendar.end()) {
                lock.unlock();
                thread_::_this.sleep_for(idle_wakeup);
                continue;
            }
            uint64_t tip_ts=duration_cast<nanoseconds>((system_clock::now()-
                                              relay_interval).time_since_epoch()).count();
            if (i->first>tip_ts) {
                lock.unlock();
                thread_::_this.sleep_for(idle_wakeup);
                continue;
            }
            e=i->second.first;
            p=i->second.second;
            m_calendar.erase(i);
        }
        if (e->get_ts() < last_ev_ts) { 
            delete e;
            continue;
        }
        p->process(*e);
        last_ev_ts=e->get_ts();
        delete e;
    }
}

void daemon::run() {
    thread peers(&networking::run,&m_peerd);
    thread tep(&daemon::evidence_processor, this);

    vector<thread> apps_threads;
    apps_threads.reserve(m_apps_.size());
    for (auto& i:m_apps_) {
        auto p=dynamic_cast<runnable_app*>(i.second);
        if (p!=0) 
            apps_threads.emplace_back(thread(&runnable_app::run,p));
    }

    signal_handler::_this.add(&m_syncd);
    thread synct(&syncd_t::run,&m_syncd);

    thread_::_this.sleep_for(chrono::seconds(5));

    assert(m_pool==0);
    m_pool=new diff();

    load_head();

    while(!program::_this.terminated) { 
        m_cycle.wait_for_stage(cycle_t::new_cycle);
        stage1(m_cycle);

        m_cycle.wait_for_stage(cycle_t::local_deltas_io);
        stage2(m_cycle);

        m_cycle.wait_for_stage(cycle_t::sync_db);
        if (!stage3(m_cycle)) 
            continue;

        m_cycle.wait_for_stage(cycle_t::consensus_vote_tip_io);
        stage4(m_cycle);
    }

    synct.join();
    for (auto& i:apps_threads) 
        i.join();
    tep.join();
    peers.join();
}

string daemon::get_random_node(const unordered_set<string>& exclude_addrs) const {
    auto s=m_auth_app->get_random_node(m_rng,exclude_addrs);
    return s;
}

bool daemon::get_prev(const string& filename, diff::hash_t& prev) const {
    if (unlikely(filename.empty())) 
        return false;
    ifstream is(filename);
    if (!is.good()) 
        return false;
    is >> prev;
    return true;
}

peer_t* daemon::get_random_edge() const {
    auto n=get_nodes(); 
    cout << n.size() << " nodes available" << endl;
    if (n.empty()) 
        return 0;
    uniform_int_distribution<> d(0, n.size()-1);
    auto i=n.begin();
    advance(i,d(m_rng));
    return *i;
}

peer_t* daemon::get_random_edge(const peer_t* exclude) const {
    auto n=get_nodes(exclude); 
    cout << n.size() << " nodes available" << endl;
    if (n.empty()) 
        return 0;
    uniform_int_distribution<> d(0, n.size()-1);
    auto i=n.begin();
    advance(i,d(m_rng));
    return *i;
}

vector<peer_t*> daemon::get_nodes(const peer_t* exclude) const {
    vector<peer_t*> v;
    for (auto& i:m_peerd.in_service()) {
        auto p=static_cast<peer_t*>(i);
        if (p != exclude && p->get_stage()==peer_t::node) 
            v.push_back(p);
    }
    return v;
}

vector<peer_t*> daemon::get_nodes() const{
    vector<peer_t*> v;
    for (auto& i:m_peerd.in_service()) {
        auto p=static_cast<peer_t*>(i);
        if (p->get_stage()==peer_t::node) 
        v.push_back(p);
    }
    return v;
}

vector<peer_t*> daemon::get_people() {
    vector<peer_t*> v;
    for (auto& i:m_peerd.in_service()) {
        auto p=reinterpret_cast<peer_t*>(i);
        if (p->get_stage()==peer_t::out || p->get_stage()==peer_t::hall 
                                  || p->get_stage()==peer_t::node) v.push_back(p);
    }
    return v;
}

void daemon::update_peers_state() {
    for (auto& i:m_peerd.in_service()) {
        auto p=reinterpret_cast<peer_t*>(i);
        if (p->get_stage()!=peer_t::sysop) {
            p->set_stage(m_auth_app->get_db().get_stage(p->get_pubkey().hash()));
        }
    }
}

void daemon::send(const local_deltas& g, peer_t* exclude) {
    ostringstream os;
    g.to_stream(os);
    string msg=os.str();
    datagram d(protocol::local_deltas,msg);
    for (auto& i:m_peerd.get_nodes()) {
        if (i==exclude) 
            continue; 
        i->send(d);
    }
}

local_deltas* daemon::create_local_deltas() {
    auto* mg=new local_deltas();
    {
        lock_guard<mutex> lock(m_peerd.m_mx_evidences);
        for (auto&i:m_apps_) {
            auto* amg=i.second->create_local_delta(); 
            if (amg!=0) {
                mg->emplace(i.first,amg);
            }
        }
        auto e=m_peerd.retrieve_evidences();
        delete e;
    }
    if (mg->empty()) {
        delete mg;
        return 0;
    }
    mg->sign(m_id);
    return mg;
}

void daemon::vote_tip(const diff& b) {
    const diff::hash_t& h=b.hash();
    m_votes.add(m_id.get_pubkey().hash(),h);

    cout << "voting for tip: " << h << endl;
    string signature=crypto::ec::get_instance().sign_encode(m_id.get_privkey(),h.to_b58());
    ostringstream os;
    os << h << " " << m_id.get_pubkey() << " " << signature;
    string msg=os.str();
    for (auto& i:get_people()) {
        i->send(new datagram(protocol::vote_tip,msg));
    }
}

void daemon::process_vote_tip(peer_t *c, datagram*d) {
    auto s=d->parse_string();
    delete d;
    istringstream is(s);
    string block_hash_b58;
    is >> block_hash_b58;
    pubkey_t pubkey;
    is >> pubkey;
    string signature;
    is >> signature;
    cout << "Received vote from " << pubkey 
         << " head be " << block_hash_b58 << endl;
    if (!crypto::ec::get_instance().verify(pubkey, block_hash_b58, signature)) {
        cout << "WRONG SIGNATURE in vote." << endl;
        return;
    }
    if (m_votes.add(pubkey.hash(),diff::hash_t::from_b58(block_hash_b58))) {
        if (m_auth_app->my_stage()==peer_t::node) { 
            for (auto& i:get_people()) {
                if (i==c) 
                    continue;
                i->send(new datagram(protocol::vote_tip,s));
            }
        }
    }
}

void daemon::process_incoming_local_deltas(peer_t *c, datagram*d) {
    auto s=d->parse_string();
    delete d;
    istringstream is(s);
    local_deltas* g=local_deltas::from_stream(is);
    if (!g) 
        return;
    if (unlikely(!g->verify())) { 
        delete g;
        cout << "Failed verification of incoming local_deltas" << endl;
        return;
    }

    bool allowed;
    {
        lock_guard<mutex> lock(m_mx_pool);
        allowed=m_pool->allow(*g);
    }
    if (!allowed) {
        delete g;
        return;
    }
    send(*g,c);

    {
        lock_guard<mutex> lock(m_mx_pool);
        m_pool->add(g);
    }
}

void daemon::flush_evidences_on_hold() {
    unique_lock<mutex> lock(m_evidences_on_hold.mx);
    auto i=m_evidences_on_hold.begin();
    while (i!=m_evidences_on_hold.end()) {
        auto p=*i;
        lock.unlock();
        if (!process_evidence(p)) {
            cerr << "Nobody recognized this evidence" << endl;
            p->dump(cout);
            delete p;
        }
        lock.lock();
        i = m_evidences_on_hold.erase(i);
    }

    lock.unlock();
    m_evidences_on_hold.cv.notify_all();
}

void daemon::on_sync() { 
    flush_evidences_on_hold();
}

bool daemon::process_evidence(datagram*d) {
    if (!m_syncd.in_sync()) {
        cout << "blockchain: holding evidence until sync is completed" << endl;
        m_evidences_on_hold.add(d);
        return true;
    }

    assert(m_syncd.in_sync());
    string payload=d->parse_string();
    auto service = d->service;
    delete d;
    
    bool processed=false;
    for (auto&i:m_apps_) {
        auto e = i.second->parse_evidence(service, payload);
        if (e != 0) {
            processed=true;
            m_calendar.schedule(e,i.second);
            break;
        }
    }
    return processed;
}

bool daemon::process_app_query(peer_t *c, datagram*d) {
    bool processed=false;
    for (auto&i:m_apps_) {
        if (i.second->process_query(c,d)) {
            processed=true;
            break;
        }
    }
    return processed;
}


bool daemon::process_sysop_request(peer_t *c, datagram*d) {
    bool alowed_sysop=sysop_allowed;
    if (c->get_stage()==peer_t::sysop) { 
        if (!sysop_allowed) {
            delete d;
            c->disconnect();
            return true;
        }
        if (!m_sysops.process_work(c, d)) { 
            cerr << "Error , sysop request not handled" << endl;
            delete d;
        }
        return true;
    }
    delete d;
    c->disconnect();
    return true;
}

bool daemon::process_work(peer_t *c, datagram*d) {
    switch(d->service) {
        case protocol::vote_tip: {
            process_vote_tip(c,d);
            return true;
        }
        case protocol::local_deltas: {
            process_incoming_local_deltas(c,d);
            return true;
        }
        case protocol::sysop: {
            if (likely(c->get_stage()==peer_t::sysop)) { 
                if (likely(process_sysop_request(c,d))) {
                    return true;
                }
            }else { 
                cout << "unexpected sysop datagram." << endl;
                delete d;
                c->disconnect();
                return true;
            }
       }
    }
    return false;
}

void daemon::set_last_block_imported_(const diff::hash_t& h) {
    m_last_block_imported=h;
    string filename = m_home+"/head";
    ofstream os(filename);
    os << m_last_block_imported << endl;
}

void daemon::clear() {
    for (auto&i:m_apps_) {
        i.second->clear();
    }
    set_last_block_imported(0);
}

void daemon::set_last_block_imported(const diff::hash_t& h) {
    lock_guard<mutex> lock(m_mx_import);
    set_last_block_imported_(h);
}

bool daemon::import(const diff& b) {
    lock_guard<mutex> lock(m_mx_import);
    if (b.get_previews_hash()!=m_last_block_imported) {
        cout << "block not in sequence." << b.get_previews_hash() << " " << m_last_block_imported << endl;
        return false;
    }

    app::get_chain_info().set_imported_last_block(m_last_block_imported);

    for (auto&i:b) {
        auto a=m_apps_.find(i.first);
        assert(a!=m_apps_.end());
        assert(a->second!=0);
        a->second->import(*i.second,b.get_proof_of_work());
        if (a->second==m_auth_app) {
            update_peers_state();
        }
    }
    set_last_block_imported_(b.hash());
    return true;
}

void daemon::votes_t::clear() {
    lock_guard<mutex> lock(mx);
    b::clear();
}

bool daemon::votes_t::add(const pubkey_t::hash_t& h,const diff::hash_t& v) {
    auto i=find(h);
    if (i!=end()) {
        return false; 
    }
    emplace(h,make_pair(v,1));
    return true;
}

diff::hash_t daemon::votes_t::select() {
    map<diff::hash_t,unsigned long> x;
    {
        lock_guard<mutex> lock(mx);
        if (empty()) 
            return diff::hash_t(0);
        for (auto&i:*this) {
            auto a=x.find(i.second.first);
            if (a!=x.end()) {
                a->second++;
            }else {
                x.emplace(i.second.first,1);
            }
        }
    }

    unsigned long max{0};
    const diff::hash_t* hash;
    for (auto&i:x) {
        if (i.second>=max) {
            max=i.second;
            hash=&(i.first);
        }
    }
    auto ans=*hash;
    b::clear();
    return move(ans);
}

bool daemon::sysops_t::process_work(peer_t *p, datagram*d) {
    if (d->service!=protocol::sysop) 
        return false;
    lock_guard<mutex> lock(mx);
    auto i=find(p);
    if (i==end()) {
        i=emplace(p,shell(this->d)).first;
    }
    string response=i->second.command(d->parse_string());
    delete d;
    p->send(new datagram(us::gov::protocol::sysop,response));
    return true;
}

string daemon::apps::shell_command(int app_id, const string& cmdline) const {
    auto i=find(app_id);
    if (i==end()) 
        return "app not found";
    return i->second->shell_command(cmdline);
}

string daemon::shell_command(int app_id, const string& cmdline) const {
    return m_apps_.shell_command(app_id,cmdline);
}

void daemon::apps::dump(ostream& os) const {
    for (auto&i:*this) {
        os << i.first << '\t' << i.second->get_name() << endl;
    }
}

void daemon::list_apps(ostream& os) const {
    m_apps_.dump(os);
}

void daemon::dump(ostream& os) const {
    os << "Hello from engine::daemon" << endl;
    os << "networking" << endl;
    m_peerd.dump(os);
}

string daemon::timestamp() const {
    return "NOW"; 
}

void daemon::print_performances(ostream& os) const {
    os << "US node " << m_id.get_pubkey() << " " << timestamp() << endl;
    os << "Resources" << endl;
    os << "  Network" << endl;
    os << "  Storage" << endl;
    os << "  CPU" << endl;
    os << "Time series by indicator" << endl;
    os << "  DiffHash Histogram" << endl;
    os << "       current: #1st-80%, #2nd-05%" << endl;
    os << "        1m ago: #1st-80%, #2nd-05%" << endl;
    os << "        2m ago: #1st-80%, #2nd-05%" << endl;
    os << "        3m ago: #1st-80%, #2nd-05%" << endl;
    os << "        4m ago: #1st-80%, #2nd-05%" << endl;
    os << "        5m ago: #1st-80%, #2nd-05%" << endl;
    os << "  51% vote power attack" << endl;
    os << "       current: 0%" << endl;
    os << "        1m ago: 0%" << endl;
    os << "        2m ago: 0%" << endl;
    os << "        3m ago: 0%" << endl;
    os << "        4m ago: 0%" << endl;
    os << "        5m ago: 0%" << endl;
    os << "  Missed tip" << endl;
    os << "       current: No" << endl;
    os << "        1m ago: No" << endl;
    os << "        2m ago: No" << endl;
    os << "        3m ago: No" << endl;
    os << "        4m ago: Yes" << endl;
    os << "        5m ago: No" << endl;
}
