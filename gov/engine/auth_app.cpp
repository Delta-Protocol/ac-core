#include "auth_app.h"
#include <random>
#include "daemon.h"

using namespace std;
using namespace us::gov;
using namespace us::gov::engine::auth;

constexpr array<const char*,policies_traits::num_params> policies_traits::m_paramstr;

app::app(const pubkey_t& pk): m_node_pubkey(pk) {
    m_pool=new app::local_delta();
    m_policies_local=m_policies;
}

app::~app() {
    delete m_pool;
}

us::gov::engine::peer_t::stage_t app::my_stage() const {
    auto k = m_node_pubkey.hash();
    if(m_db.m_nodes.find(k)!=m_db.m_nodes.end()){
        m_cache_my_stage=peer_t::node;
    }else if(m_db.m_hall.find(k)!=m_db.m_hall.end()){
        m_cache_my_stage=peer_t::hall;
    }else{
        m_cache_my_stage=peer_t::out;
    }
    return m_cache_my_stage;
}

void app::run() {
    while(!program::_this.terminated) {
        m_db.dump(cout);
        thread_::_this.sleep_for(chrono::seconds(30));
    }
}

void app::basic_auth_completed(peer_t* p) {
    if (p->get_pubkey()==m_node_pubkey){ 
        p->set_stage(peer_t::sysop);
        return;
    }
    {
        lock_guard<mutex> lock(m_db.m_mx_nodes);
        auto i=m_db.m_nodes.find(p->get_pubkey().hash());
        if (i!=m_db.m_nodes.end()){
            p->set_stage(peer_t::node);
            return;
        }
    }
    {
        db_t::nodes_t::const_iterator i;
        {
            lock_guard<mutex> lock(m_db.m_mx_hall);
            i=m_db.m_hall.find(p->get_pubkey().hash());
        }
        if (i==m_db.m_hall.end()){
            p->set_stage(peer_t::out);
            m_pool->m_to_hall.push_back(make_pair(p->get_pubkey().hash(),p->addr));
        }else{
            p->set_stage(peer_t::hall);
        }
    }
}

void app::add_policies() {
    lock_guard<mutex> lock(m_mx_pool);
    lock_guard<mutex> lock2(m_mx_policies);
    for (int i=0; i<policies_traits::num_params; ++i)
        (*m_pool)[i] = m_policies_local[i];
}

void app::add_growth_transactions(unsigned int seed) {

    double growth=m_policies[policies_traits::network_growth];
    if (abs(growth)<1e-8) 
        return;

    int min_growth=round(m_policies[policies_traits::network_min_growth]);
    default_random_engine generator(seed);
    db_t::nodes_t* src;
    db_t::nodes_t* dst;
    unordered_set<int> uniq;
    size_t maxr;
    int s;
    lock_guard<mutex> lock(m_db.m_mx_hall);
    lock_guard<mutex> lock2(m_db.m_mx_nodes);

    if (growth>=0) {
        size_t nh=m_db.m_hall.size();
        s=floor((double)nh*growth);
        src=&m_db.m_hall;
        dst=&m_db.m_nodes;
        if (s<min_growth) 
            s=min_growth;
        if (s>nh) 
            s=nh;
        maxr=nh-1;
    }else{
        size_t nn=m_db.m_nodes.size();
        s = -floor((double)nn*growth);
        src=&m_db.m_nodes;
        dst=&m_db.m_hall;
        maxr=nn-1;
    }
    uniform_int_distribution<size_t> distribution(0,maxr);
    
    for (size_t i=0; i<s; ++i) {
        auto p=src->begin();
        size_t r;
        while (true) {
            r=distribution(generator);
            if (!uniq.insert(r).second || r>=src->size()) 
                continue;
            break;
        }
        advance(p,r);
        dst->emplace(*p);
        src->erase(p);
    }
}

int us::gov::engine::auth::app::local_delta::app_id() const {
    return app::id();
}

void us::gov::engine::auth::app::local_delta::to_stream(ostream& os) const {
    os << m_to_hall.size() << " ";
    for (auto& i:m_to_hall) {
        os << i.first << " " << i.second << " ";
    }
    b::to_stream(os);
}

void us::gov::engine::auth::app::local_delta::from_stream(istream& is) {
    int n;
    is >> n;
    m_to_hall.reserve(n);
    for (int i=0; i<n; ++i) {
        pubkey_t::hash_t pkeyh;
        address addr;
        is >> pkeyh;
        is >> addr;
        m_to_hall.push_back(make_pair(pkeyh,addr));
    }
    b::from_stream(is);
}

void us::gov::engine::auth::app::delta::to_stream(ostream& os) const {
    os << m_to_hall.size() << " ";
    for (auto& i:m_to_hall) {
        os << i.first << " " << i.second << " ";
    }
    b::base_1::to_stream(os);
}

app::delta* us::gov::engine::auth::app::delta::from_stream(istream& is) {
    delta* g=new delta();
    {
        int n;
        is >> n;
        for (int i=0; i<n; ++i) {
            pubkey_t::hash_t pkeyh;
            address addr;
            is >> pkeyh;
            is >> addr;
            g->m_to_hall.emplace(make_pair(pkeyh,addr));
        }
    }
    static_cast<b*>(g)->from_stream(is);
    return g;
}

void app::import(const engine::app::delta& gg, const engine::pow_t&) {
    const delta& g=static_cast<const delta&>(gg);
    {
        lock_guard<mutex> lock(m_db.m_mx_hall);
        for (auto& i:g.m_to_hall) {
            auto k=m_db.m_hall.find(i.first);
            if (k==m_db.m_hall.end()) 
                m_db.m_hall.emplace(i);
        }
    }
    {
        lock_guard<mutex> lock(m_mx_policies);
        for (int i=0; i<policies_traits::num_params; ++i) 
            m_policies[i]=g[i];
    }
    add_growth_transactions(get_seed());
    m_cache_my_stage=peer_t::unknown;
}

engine::peer_t::stage_t 
app::db_t::get_stage(const pubkeyh_t& key) const {
    {
        lock_guard<mutex> lock(m_mx_nodes);
        if (m_nodes.find(key)!=m_nodes.end()) 
            return peer_t::node;
    }
    {
        lock_guard<mutex> lock(m_mx_hall);
        if (m_hall.find(key)!=m_hall.end()) 
            return peer_t::hall;
    }
    return peer_t::out;
}

us::gov::engine::app::local_delta* 
app::create_local_delta() {
    add_policies();
    lock_guard<mutex> lock(m_mx_pool);
    auto full=m_pool;
    m_pool=new app::local_delta();
    return full; 
}

void app::db_t::dump(ostream& os) const {
    {
        lock_guard<mutex> lock(m_mx_nodes);
        cout << m_nodes.size() << " nodes:" << endl;
        for (auto& i:m_nodes) {
            cout << "  " << i.first << " " << i.second << endl;
        }
    }
    {
        lock_guard<mutex> lock(m_mx_hall);
        cout << m_hall.size() << " candidates in hall:" << endl;
        for (auto& i:m_hall) {
            cout << "  " << i.first << " " << i.second << endl;
        }
    }
}

string app::get_random_node(mt19937_64& rng, const unordered_set<string>& exclude_addrs) const {
    lock_guard<mutex> lock(m_db.m_mx_nodes);
    if (m_db.m_nodes.empty()) 
        return "";
    uniform_int_distribution<> d(0, m_db.m_nodes.size()-1);
    for (int j=0; j<10; ++j) {
        auto i=m_db.m_nodes.begin();
        advance(i,d(rng));
        if (i->first!=m_node_pubkey.hash() && 
                    exclude_addrs.find(i->second)==exclude_addrs.end()) {
            return i->second;
        }
    }
    return "";
}

string app::shell_command(const string& cmdline) {
    ostringstream os;
    istringstream is(cmdline);
    string cmd;
    is >> cmd;
    if (cmd=="hello") {
        os << "Auth app shell. type h for help." << endl;
    }else if (cmd=="h" || cmd=="help") {
        os << "Auth app shell." << endl;
        os << "h|help              Shows this help." << endl;
        os << "p|policies [id vote]          ."      << endl;
        os << "s|server            Nodes,hall."      << endl;
        os << "exit                Exits this app and returns to parent shell." << endl;
        os << "" << endl;
    }else if (cmd=="exit") {
    }else if (cmd=="p" || cmd=="policies") {
        int n=-1;
        double value;
        is >> n;
        is >> value;
        if (n>=0 && n<policies_traits::num_params) {
            lock_guard<mutex> lock(m_mx_policies);
            m_policies_local[n]=value;
        }else {
            os << "parameter " << n << " not found" << endl;
        }
        dump_policies(os);
    }else if (cmd=="s" || cmd=="server") {
        m_db.dump(os);
        os << "I am " << peer_t::stagestr[my_stage()] << endl;
    }else {
        os << "Unrecognized command" << endl;
    }
    return os.str();
}

void app::dump_policies(ostream& os) const {
    lock_guard<mutex> lock(m_mx_policies);
    os << policies_traits::num_params << " consensus variables:" << endl;
    for (int i=0; i<policies_traits::num_params; ++i) {
        os << "  " << i << ". " << policies_traits::m_paramstr[i] 
           << " [avg] consensus value: " << m_policies[i] 
           << " local value:" << m_policies_local[i] << endl;
    }
}

void app::dbhash(hasher_t& h) const {
    m_db.hash(h);
    lock_guard<mutex> lock(m_mx_policies);
    m_policies.hash(h);
}

void app::db_t::hash(hasher_t& h) const {
    {
        lock_guard<mutex> lock(m_mx_nodes);
        for (auto& i:m_nodes) {
            h << i.first << i.second;
        }
    }
    {
        lock_guard<mutex> lock(m_mx_hall);
        for (auto& i:m_hall) {
            h << i.first << i.second;
        }
    }
}

void app::db_t::clear() {
    {
        lock_guard<mutex> lock(m_mx_nodes);
        m_nodes.clear();
    }
    {
        lock_guard<mutex> lock(m_mx_hall);
        m_hall.clear();
    }
}


void app::clear() {
    {
        lock_guard<mutex> lock(m_mx_policies);
        m_policies.clear();
    }
    m_db.clear();
}





