#include "app.h"
#include "protocol.h"
#include <thread>
#include <chrono>
#include <us/gov/crypto/base58.h>
#include <us/gov/likely.h>
#include "tx.h"
#include <us/gov/engine/daemon.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "locking_programs/p2pkh.h"

using namespace us::gov;
using namespace us::gov::cash;
using namespace std;

constexpr const char* app::m_name;
constexpr array<const char*,app::policies_traits::num_params> app::policies_traits::m_paramstr;

namespace std {

    template <>
    struct hash<us::gov::cash::app::local_delta> {
        size_t operator()(const us::gov::cash::app::local_delta&) const;
    };
}

size_t std::hash<us::gov::cash::app::local_delta>::operator() (
                    const us::gov::cash::app::local_delta&g) const {
    return *reinterpret_cast<const size_t*>(&g.get_hash()[0]); 
}

void app::local_delta::accounts_t::add_input(tx& t, const hash_t& addr, 
                                                    const cash_t& amount) {
    t.add_input(addr, amount);
}

void app::local_delta::accounts_t::add_output(tx& t, const hash_t& addr, 
                                                     const box_t& amount, 
                                                     const hash_t& locking_program) {
    t.add_output(addr,amount,locking_program);
}

int app::local_delta::app_id() const {
    return app::id();
}

app::app() {
    m_pool=new app::local_delta();
    m_policies_local=m_policies;
}

app::~app() {
    delete m_pool;
}

/*
//relay rules: **might be outdated**  TODO transfer to doc
//objective: 1 transaction must appear in only a few guts in the block, if every tx appears in every gut of each miner 
  the block would have size O(N*T) where N is number of miners and T number of incoming transactions for this height.
//we want block size O(N), each miner shall provide only a small % of transactions.
//For doing so, propagation of each tx shall be limited.
//The following rule encourages a miner to both relay the tx and stop relaying it after reached a maximum number of hops is reached.
//merchants start a transaction using only 3 random nodes, no more, no less, otherwise the tx might not progress to the blockchain.
//at this point the tx enters the ecosystem, 3 different propagation threads has been initiated, nodes will assign hop number 0.
//propagation threads consist on 4 hops.
//according to the rule a total of 12 nodes see the same tx.
//block validation will check how many guts from different miners contain the same transaction
//the tx will be discarded if this number is below 6 (some miner refused to relay, trying to maximize chances of getting its fee)
//the tx will be discarded if this number is above 12 (some miner relayed in excess, increasing system congestion)
//Particularity: on reception the node will check if it has seen the tx before.
//if seen before, it relays it without incrementing the hop number, so every propagation thread can provide 4 different guts containing the tx.
//in total a maximum of 12 guts is expected when making the closure block
//this miner has 1/16 chances of getting paid the fee for this tx.
bool first_time=buffer.add(t);
uint8_t hop=tx::get_hop(*d);
*/

void app::add_policies() {
    lock_guard<mutex> lock(m_mx_pool);
    lock_guard<mutex> lock2(m_mx_policies);
    for (int i=0; i<policies_traits::num_params; ++i)
        (*m_pool)[i] = m_policies_local[i];
}

engine::app::local_delta* app::create_local_delta() {
    add_policies();
    lock_guard<mutex> lock(m_mx_pool);
    auto full=m_pool;
    m_pool=new app::local_delta();
    return full;
}

bool app::process_query(peer_t* c, datagram* d) {
    switch(d->service) {
        case protocol::cash_query: {
            cash_query(c,d);
            return true;
        }
        break;
    }
    return false;
}

bool app::basic_check(const tx& t) const {

    cash_t min_fee;
    {
        lock_guard<mutex> lock(m_mx_policies);
        min_fee=m_policies[policies_traits::minimum_fee];
    }

    if (min_fee<0) {
        cerr << "TX REJECTED 2" << endl;
        return false;
    }

    auto fee=t.check();

    if (fee<min_fee) {
        cerr << "TX 3 " << fee << " " << min_fee << endl;
        cout << "RBF removed min fee check" << endl;
        return false;
    }

    return true;
}

engine::evidence* app::parse_evidence(uint16_t service, const string& datagram_payload) const {
    switch(service) {
        case protocol::cash_tx: {
            auto t=tx::from_b58(datagram_payload);
            if (unlikely(!t.first.empty())) {
               return 0;
            }
            if(!basic_check(*t.second))
                return 0;
           return t.second.release();
        } 
        break;
    }
    return 0;
}

datagram* app::query_accounts_t::get_datagram() const {
    if (empty()) return 0;
    ostringstream os;
    os << size() << ' ';
    for (auto&i:*this) {
        os << i << ' ';
    }
    return new socket::datagram(protocol::cash_query,os.str());
}

app::query_accounts_t app::query_accounts_t::from_string(const string&s) {
    int n;
    istringstream is(s);
    is >> n;
    query_accounts_t r;
    r.reserve(n);
    for (int i=0; i<n; ++i) {
        hash_t v;
        is >> v;
        r.emplace_back(move(v));
    }
    return move(r);
}

app::query_accounts_t app::query_accounts_t::from_datagram(datagram*d) {
    string query=d->parse_string();
    delete d;
    return from_string(query);
}


void app::cash_query(peer_t *c, datagram*d) {
    query_accounts_t q = query_accounts_t::from_datagram(d);
    ostringstream os;
    os << "0 "; 
    {
        lock_guard<mutex> lock(db.get_mx());
        for (auto&addr:q) {
            auto a=db.get_accounts()->find(addr);
            if (likely(a!=db.get_accounts()->end())) {
                a->second.to_stream(os);
            }else {
                account_t a;
                a.set_locking_program(0);
                a.to_stream(os);
            }
        }
    }

    os << ' ' << app::get_chain_info().get_tip(); 
    c->send(new datagram(protocol::cash_response,os.str()));
}

void app::dbg_output() const {
    while(!program::_this.terminated) {
        db.dump(cout);
        thread_::_this.sleep_for(chrono::seconds(15));
    }
}

void app::run() {
    dbg_output();
}

void app::local_delta::account_t::dump(ostream& os) const {
    os << "locking_program " << m_locking_program << "; balance " << m_box;
}

void app::local_delta::accounts_t::dump(ostream& os) const {
    cout << size() << " p2sh accounts:" << endl;
    for (auto& i:*this) {
        cout << ' ' << i.first << ' ';
        i.second.dump(os);
        os << endl;
    }
}

void app::local_delta::tokens_t::dump(ostream& os) const {}

void app::local_delta::tokens_t::to_stream(ostream& os) const {}

app::local_delta::tokens_t app::local_delta::tokens_t::from_stream(istream& is) {
    return tokens_t();
}

void app::local_delta::safe_deposit_box::dump(ostream& os) const {}

void app::local_delta::safe_deposit_box::to_stream(ostream& os) const {}

app::local_delta::safe_deposit_box 
app::local_delta::safe_deposit_box::from_stream(istream& is) {
    return safe_deposit_box();
}

void app::db_t::dump(ostream& os) const {
    cout << "cash app db dump" << endl;
    lock_guard<mutex> lock(m_mx);
    m_accounts->dump(os);
    cout << "supply_left " << m_supply_left 
         << " block_reward " << m_block_reward << endl;
}

void app::db_t::to_stream(ostream&os) const {
    m_accounts->to_stream(os);
    os << ' ';
    os << m_supply_left << ' ' << m_block_reward;
}

app::db_t app::db_t::from_stream(istream& is) {
    db_t db;
    db.m_accounts=accounts_t::from_stream(is);
    is >> db.m_supply_left;
    is >> db.m_block_reward;
    return move(db);
}

bool app::local_delta::accounts_t::pay(const hash_t& address, const box_t& amount) { 
    if (amount<=0) return false;
    auto i=find(address);
    if (i==end()) {
		emplace(make_pair(address,account_t{1,amount}));
    }else {
        i->second.set_box(i->second.get_box()+amount);
    }
    return true;
}

bool app::local_delta::accounts_t::withdraw(const hash_t& address, const box_t& amount) { 
    auto i=find(address);
    if (i==end()) {
        return false;
    }else if (i->second.get_box()<amount) {
        return false;
    }

    i->second.set_box(i->second.get_box()-amount);
    if (unlikely(i->second.get_box()==0)) {
        erase(i);
    }
    return true;
}

bool app::db_t::add_(const hash_t& k, const box_t& amount) { 
     return m_accounts->pay(k,amount);
}

bool app::db_t::withdraw_(const hash_t& k, const box_t& amount) { 
    return m_accounts->withdraw(k,amount);
}

cash_t app::db_t::get_newcash() { 
    if (likely(m_supply_left >= m_block_reward)) {
        m_supply_left-=m_block_reward;
    }else {
        m_block_reward=m_supply_left;
        m_supply_left=0;
    }
    return m_block_reward;
}

void app::import(const engine::app::delta& gg, const engine::pow_t& w) {

    const delta& g=static_cast<const delta&>(gg);
    {
        lock_guard<mutex> lock(m_mx_policies);
        for (int i=0; i<policies_traits::num_params; ++i) m_policies[i]=g[i];
    }

    {
        const app::local_delta::accounts_t& a=g.get_local_delta().get_accounts();
        lock_guard<mutex> lock(db.get_mx());
        for (auto& i:a) {
            auto d=db.m_accounts->find(i.first);
            if (d==db.m_accounts->end()) {
                if (likely(i.second.get_box()!=0)) {
                    db.m_accounts->emplace(i.first,account_t(i.second.get_locking_program(),i.second.get_box()));
                }
            }else {
                if (i.second.get_box()==0) {
                    db.m_accounts->erase(d);
                }else {
                    d->second.set_locking_program(i.second.get_locking_program());
                    d->second.set_box(i.second.get_box());
                }
            }
         }
    }

    if (likely(!w.empty())) {
        lock_guard<mutex> lock(db.m_mx);
        cash_t newcash=db.get_newcash();
        cash_t total_cash_int=newcash+g.get_local_delta().get_fees();
        double total_cash=(double)total_cash_int;
        uint64_t total_work=w.sum();
        cash_t paid=0;
        for (auto&i:w) {
            double rel_work;
            if (likely(total_work>0)) {
                rel_work=(double)i.second/(double)total_work;
            }else {
                rel_work=1.0/(double)w.size();
            }
            cash_t amount=total_cash*rel_work;
            db.add_(i.first,amount);
            paid+=amount;
        }
        cash_t remainder=total_cash_int-paid;
        if (remainder>0) {
            default_random_engine generator(get_seed());
            uniform_int_distribution<size_t> distribution(0,w.size()-1);
            auto i=w.begin();
            advance(i,distribution(generator));
            db.add_(i->first,remainder);
        }
    }
}

bool app::unlock(const hash_t& address, 
               const size_t& this_index, 
               const hash_t& locking_program, 
               const string& locking_program_input, const tx& t) {

    if (likely(locking_program<min_locking_program)) {
        if (unlikely(locking_program==0)) {
            return true;
        }else if (locking_program==p2pkh::locking_program_hash) {
            return p2pkh::check_input(address, t,this_index, locking_program_input);
        }
    }

    ostringstream prog;
    prog << "--private /home/mmayorga/.bitcoin++/locking/" << locking_program << "/main";
    
    pid_t pid = fork();
    switch (pid) {
        case -1: 
            cerr << "Uh-Oh! fork() failed.\n";
            exit(1);
        case 0: 
            cout << "firejail " << prog.str().c_str() << "\n";
            execl("firejail",prog.str().c_str()); 
            cerr << "Uh-Oh! execl() failed!";  
            exit(1);
        default:
            cout << "Process created with pid " << pid << "\n";
            int status;

        while (!WIFEXITED(status)) {
            waitpid(pid, &status, 0); 
        }
        std::cout << "Process exited with " << WEXITSTATUS(status) << "\n";
        return status==0;
    }
}

bool app::account_state(const local_delta::batch_t& batch, 
                      const hash_t& address, account_t& acc) const {
    auto  b=batch.find(address);
    if (likely(b==batch.end())) { 
        auto p=m_pool->get_accounts().find(address);
        if (likely(p==m_pool->get_accounts().end())) { 
            lock_guard<mutex> lock(db.m_mx);
            auto o=db.m_accounts->find(address);
            if (o==db.m_accounts->end())
                return false;
            
            acc.set_locking_program(o->second.get_locking_program());
            acc.set_box(o->second.get_box());
        }else { 
            acc.set_locking_program(p->second.get_locking_program());
            acc.set_box(p->second.get_box());
        }
    }else { 
        acc.set_locking_program(b->second.get_locking_program());
        acc.set_box(b->second.get_box());
   }
   return true;
}

void app::process(const evidence& e) {

    auto t = static_cast<const tx&>(e);
    local_delta::batch_t batch;
    account_t state;

    for (size_t j=0; j<t.get_inputs().size(); ++j) {
        auto& i=t.get_inputs()[j];
        if (!account_state(batch,i.get_address(),state)) {
            cerr << "TX REJECTED " << endl;
            return;
        }

        if (!unlock(i.get_address(), j,state.get_locking_program(), i.get_locking_program_input(), t)) {
           cerr << "TX REJECTED 4" << endl;
           return;
        }
        state.set_box(state.get_box()-i.get_amount());
        batch.add(i.get_address(),state);
    }

    for (size_t j=0; j<t.get_outputs().size(); ++j) {
        auto& i=t.get_outputs()[j];
        if (account_state(batch,i.get_address(),state)) {
            if (unlikely(state.get_box()!=0 && state.get_locking_program()!=i.get_locking_program())) { 
                cerr << "TX REJECTED 5" << endl;
                return;
            }
            state.set_box(state.get_box()+i.get_amount());
        }else { 
            state.set_locking_program(i.get_locking_program());
            state.set_box(i.get_amount());
        }
        batch.add(i.get_address(),state);
    }
    auto fee=t.check();
    m_pool->set_fees(m_pool->get_fees() + fee);
    m_pool->add(batch);
}

void app::local_delta::to_stream(ostream& os) const {
    os << m_accounts.size() << " ";
    for (auto& i:m_accounts) {
        os << i.first << ' ';
        i.second.to_stream(os);
    }
    os << m_fees << ' ';
    b::to_stream(os);
}

void app::local_delta::from_stream(istream& is) {
    int n;
    is >> n;
    for (int i=0; i<n; ++i) {
        hash_t pubk;
        is >> pubk;
        batch_item bi=batch_item::from_stream(is);
        m_accounts.emplace(move(pubk),move(bi));
    }
    is >> m_fees;
    b::from_stream(is);
}

void app::delta::to_stream(ostream& os) const {
    m_local_delta.to_stream(os);
    b::base_1::to_stream(os);
}

app::delta* app::delta::from_stream(istream& is) {
    delta* g=new delta();
    g->local_delta_from_stream(is);
    static_cast<b*>(g)->from_stream(is);
    return g;
}

string app::shell_command(const string& cmdline) {
    ostringstream os;
    istringstream is(cmdline);
    string cmd;
    is >> cmd;
    if (cmd=="hello") {
        os << "cash shell. type h for help." << endl;
    }
    else if (cmd=="h" || cmd=="help") {
        os << "Cash shell." << endl;
        os << "h|help         Shows this help." << endl;
        os << "p|policies [id vote]          ." << endl;
        os << "mempool      Dumps the mempool." << endl;
        os << "exit         Exits this app and returns to parent shell." << endl;
        os << "" << endl;
    }
    else if (cmd=="p" || cmd=="policies") {
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
    }else if (cmd=="mempool") {
        lock_guard<mutex> lock(m_mx_pool);
        m_pool->get_accounts().dump(os);
        os << "fees: " << m_pool->get_fees() << endl;
    }else if (cmd=="exit") {
    }else {
        os << "Unrecognized command" << endl;
    }
    return os.str();
}

void app::db_t::clear() {
    lock_guard<mutex> lock(m_mx);
    delete m_accounts;
    m_accounts=new accounts_t();
    m_supply_left=2100000000000000; 
    m_block_reward=500000000;
}

void app::dbhash(hasher_t&) const {
    assert(false);//TBD
}

void app::clear() {
    {
        lock_guard<mutex> lock(m_mx_policies);
        m_policies.clear();
    }
    db.clear();
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

app::local_delta& app::local_delta::operator =(int zero) {
    *this=local_delta();
    return *this;
}

app::local_delta::local_delta() {}

app::local_delta::~local_delta() {}

app::local_delta::account_t::account_t(): m_box(0) {}

app::local_delta::account_t::account_t(const hash_t& locking_program, 
                                       const box_t& box): 
                                           m_locking_program(locking_program), m_box(box) {
}

void app::local_delta::account_t::to_stream(ostream& os) const {
    os << m_locking_program << ' ' << m_box << ' ';
}

app::local_delta::account_t 
app::local_delta::account_t::from_stream(istream& is) {
    account_t i;
    is >> i.m_locking_program;
    is >> i.m_box;
    return move(i);
}

void app::local_delta::batch_t::add(const hash_t& address, const batch_item& bi) {
    auto i=find(address);
    if (likely(i==end())) {
        emplace(address,bi);
    }else {
        i->second=bi;
    }
}

void app::local_delta::accounts_t::add(const batch_t& batch) {
    for (auto& b:batch) {
        auto i=find(b.first);
        if (likely(i==end())) {
            emplace(b);
        }else {
            i->second=b.second;
        }
    }
}

app::local_delta::box_t 
app::local_delta::accounts_t::get_balance() const {
    box_t b=0;
    for (auto&i:*this) {
        b+=i.second.get_box();
    }
    return move(b);
}

void app::local_delta::accounts_t::to_stream(ostream& os) const {
    os << size() << ' ';
    for (auto& i:*this) {
        os << i.first << ' ';
        i.second.to_stream(os);
    }
}
app::local_delta::accounts_t* 
app::local_delta::accounts_t::from_stream(istream& is) {
    accounts_t* r=new accounts_t();
    size_t n;
    is >> n;
    for (size_t i=0; i<n; ++i) {
        hash_t h;
        is >> h;
        r->emplace(h,move(account_t::from_stream(is)));
    }
    return r;
}

const hash_t& app::local_delta::get_hash() const {
    if (m_hash==0) 
        m_hash=compute_hash();
     return m_hash;
}

hash_t app::local_delta::compute_hash() const { 
    hasher_t h;
    for (auto&i:m_accounts) {
        h.write(i.first);
        h.write(i.second.get_box());
        h.write(i.second.get_locking_program());
    }
    h.write(m_fees);
    hash_t v;
    h.finalize(v);
    return move(v);
}

uint64_t app::delta::merge(engine::app::local_delta* other0) {
    local_delta* other=static_cast<local_delta*>(other0);
    m_m->merge(*other,*other);
    b::merge(other0);
    return 0; 
}

void app::delta::end_merge() {
    m_m->end_merge(m_local_delta, 0);
    b::end_merge();
}

app::delta::delta():m_m(new engine::majority_merger<local_delta>) {}

app::delta::~delta() {
    delete m_m;
}

app::db_t::db_t() {
    clear();
}

app::db_t::db_t(db_t&& other):
        m_supply_left(other.m_supply_left), 
        m_block_reward(other.m_block_reward) {
    m_accounts=other.m_accounts;
    other.m_accounts=0;
}

app::db_t::~db_t() {
    delete m_accounts;
}

