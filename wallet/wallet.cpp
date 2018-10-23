#include "wallet.h"
#include "protocol.h"
#include <us/gov/input.h>
#include <us/gov/protocol.h>
#include <us/gov/cash/locking_programs/p2pkh.h>

using namespace us::wallet;
using namespace std;
using socket::datagram;

wallet::wallet(const string& datapath, 
               const string& backend_host, 
               uint16_t backend_port):
                  m_datapath(datapath), 
                  m_backend_host(backend_host), 
                  m_backend_port(backend_port){
    load();
}

wallet::~wallet() {
    save();
}

string wallet::filename() const {
    auto file=m_datapath+"/keys";
    return file;
}

bool wallet::file_exists() const {
    return us::gov::input::cfg::file_exists(filename());
}

bool wallet::load() {
    auto file=filename();
    if (!us::gov::input::cfg::file_exists(file)) 
        return false;
    ifstream f(file);
    while(f.good()) {
        string pkb58;
        f >> pkb58;
        if (pkb58.empty()) 
            continue;
        auto pk=crypto::ec::keys::priv_t::from_b58(pkb58);
        if (!gov::crypto::ec::keys::verify(pk)) {
            cerr << "The private key " << pkb58 << " is incorrect." << endl;
            continue;
	}
        crypto::ec::keys k(pk);
        cash::hash_t h=k.get_pubkey().compute_hash(); 
        emplace(h,move(k));
    }
    return true;
}

bool wallet::save() const {
    if (!m_need_save) 
        return true;
    auto file=filename();
    ofstream f(file);
    for (auto&i:*this) {
        f << i.second.get_privkey() << ' ';
    }
    m_need_save=false;
    return true;
}

cash::hash_t wallet::new_address() {
    crypto::ec::keys k=crypto::ec::keys::generate();
    auto h=k.get_pubkey().compute_hash();
    emplace(h,move(k));
    m_need_save=true;
    save();
    return move(h);
}

cash::hash_t wallet::add_address(const crypto::ec::keys::priv_t& key) {
    if (!gov::crypto::ec::keys::verify(key)) {
        cerr << "Invalid private key" << endl;
        return 0;
    }
    crypto::ec::keys k(key);
    auto h=k.get_pubkey().compute_hash();
    if (emplace(h,move(k)).second) {
        m_need_save=true;
        save();
    }
    return move(h);
}

const crypto::ec::keys* wallet::get_keys(const cash::hash_t& address) const {
    auto i=find(address);
    if (i==end()) 
        return 0;
    return &i->second;
}

cash::cash_t wallet::balance() const {
    cash::cash_t b=0;
    for (auto& i:m_data) {
        b+=i.second.get_box();
    }
    return b;
}

void wallet::dump_balances(ostream& os) const {
    cash::cash_t b=0;
    os << "[address] [locking_program] [balance]" << endl;
    for (auto& i:m_data) {
        b+=i.second.get_box();
        os << i.first << ' ' << i.second.get_locking_program() << ' '
                             << i.second.get_box() << endl;
    }
    os << "total balance: " << b << endl;
}

void wallet::extended_balance(ostream& os) const {
    for (auto& i:m_data) {
        os << i.first << ' ' << i.second.get_locking_program() 
                             << ' ' << i.second.get_box() << endl;
    }
}

pair<string, wallet::accounts_query_t> 
wallet::query_accounts(socket::peer_t& peer, 
                       const cash::app::query_accounts_t& addresses) const {
    pair<string,accounts_query_t> ret;
    socket::datagram* d=addresses.get_datagram();
    if (unlikely(!d)) {
        ret.first="Error. Wallet contains no addresses.";
        return move(ret);
    }

    pair<string,datagram*> response = 
                          peer.send_recv(d,us::gov::protocol::cash_response);
    if (unlikely(!response.first.empty())) {
        ret.first=response.first;
        return move(ret);
    }

    if (response.second->service==gov::protocol::gov_socket_error) {
        ret.first=response.second->parse_string();
        delete response.second;
        return move(ret);
    }

    auto r=response.second->parse_string(); 
        delete response.second;

    istringstream is(r);
    int code;
    is >> code;
    if (unlikely(code!=0)) {	
        string err;
        is >> err;
        ostringstream os;
        os << "Error. Backend reported: " << err;
        ret.first=os.str();
    }else {
        for(auto&i:addresses) {
            cash::app::account_t a=cash::app::account_t::from_stream(is);
                if (a.get_box() > 0) {
                    ret.second.emplace(i,move(a));
                }
        }
    }
    return move(ret);
}

string wallet::refresh(socket::peer_t& peer) {
    cash::app::query_accounts_t addresses;
    addresses.reserve(size());
    for (auto&i:*this) {
        addresses.emplace_back(i.first);
    }
    auto r=query_accounts(peer,addresses);
    if (likely(r.first.empty())) 
        m_data=move(r.second);
    return r.first;
}

pair<string,wallet::input_accounts_t> 
wallet::select_sources(socket::peer_t& peer, const cash::cash_t& amount) {
    pair<string, wallet::input_accounts_t> ans;

    ans.first=refresh(peer);
    if (unlikely(!ans.first.empty())) {
        return move(ans);
    }

    vector<accounts_query_t::const_iterator> v;	
    v.reserve(m_data.size());
    for (accounts_query_t::const_iterator i=m_data.begin(); i!=m_data.end(); ++i) {
        v.emplace_back(i);
    }

    sort(v.begin(),v.end(),[](const accounts_query_t::const_iterator&v1,
                              const accounts_query_t::const_iterator&v2) { 
        return v1->second.get_box() < v2->second.get_box(); 
    });

    cash::cash_t remaining=amount;
    for (auto&i:v) {
        if (i->second.get_box()<=remaining) {
            ans.second.emplace_back(input_account_t(i->first,i->second,i->second.get_box()));
            remaining-=i->second.get_box();
        }else {
            ans.second.emplace_back(input_account_t(i->first,i->second,remaining));
            remaining=0;
            break;
        }
    }
    return move(ans);
}

void wallet::dump(ostream& os) const {
    os << "[private Key] [public key] [address]" << endl;
    int n=0;
    for (auto&i:*this) {
        os << '#' << n++ << ": " << i.second.get_privkey() << ' ' 
                  << i.second.get_pubkey() << ' ' << i.first << endl;
    }
}

void wallet::list(bool showpriv, ostream& os) const {
    os << "#: ";
    if (showpriv)
        os << "[private Key] ";
    os << "[public key] [address]" << endl;
    int n=0;
    if (showpriv) {
        for (auto&i:*this) {
            os << '#' << n++ << ": " << i.second.get_privkey() << ' ' 
                      << i.second.get_pubkey() << ' ' << i.first << endl;
        }
    }else {
        for (auto&i:*this) {
            os << '#' << n++ << ": " << i.second.get_pubkey() 
                                     << ' ' << i.first << endl;
        }
    }
    os << size() << " keys";
}

wallet::input_account_t::input_account_t(const hash_t& address,
                                         const cash::app::account_t& acc, 
                                         const cash_t& withdraw_amount):
                                            cash::app::account_t(acc), 
                                            m_address(address),
                                            m_withdraw_amount(withdraw_amount) {
}

void wallet::input_account_t::dump(ostream& os) const {
    os << "address " << m_address << ' ';
    cash::app::account_t::dump(os);
    os << " withdraw amount: " << m_withdraw_amount;
}

cash::cash_t wallet::input_accounts_t::get_withdraw_amount() const {
    cash::cash_t w=0;
    for (auto&i:*this) {
        w+=i.m_withdraw_amount;
    }
    return move(w);
}

void wallet::input_accounts_t::dump(ostream& os) const {
    for (auto&i:*this) {
        i.dump(os);
        os << endl;
    }
    os << "parent block: " << m_parent_block << endl;
    os << "total withdraw: " << get_withdraw_amount() << endl;
}

string wallet::send(socket::peer_t& cli, const cash::tx& t) const {
    auto fee=t.check();
    if (fee<=0) {
        return "Error. Individual inputs and fees must be positive.";
    }
    return cli.send(t.get_datagram());
}

string wallet::generate_locking_program_input(const crypto::ec::sigmsg_hasher_t::value_type& msg, 
                                              const cash::tx::sigcodes_t& sigcodes, 
                                              const cash::hash_t& address, 
                                              const cash::hash_t& locking_program) {
    if (likely(locking_program<cash::min_locking_program)) {
        if (unlikely(locking_program==0)) {
            return "";
        }else if (locking_program==1) {
            const crypto::ec::keys* k=get_keys(address);
            if (k==0) 
                return "";
            return cash::p2pkh::create_input(msg, sigcodes, k->get_privkey());
        }
    }
    return "";
}

string wallet::generate_locking_program_input(const cash::tx& t, size_t this_index, 
                                              const cash::tx::sigcodes_t& sigcodes, 
                                              const cash::hash_t& address, 
                                              const cash::hash_t& locking_program) {
    if (likely(locking_program<cash::min_locking_program)) {
        if (unlikely(locking_program==0)) {
            return "";
        }else if (locking_program==1) {
            const crypto::ec::keys* k=get_keys(address);
            if (k==0) 
                return "";
            return cash::p2pkh::create_input(t,this_index, sigcodes, k->get_privkey());
        }
    }
    return "";
}

pair<string,unique_ptr<cash::tx>> 
wallet::tx_sign(socket::peer_t& peer, 
                const string& txb58, 
                const cash::tx::sigcode_t& sigcodei, 
                const cash::tx::sigcode_t& sigcodeo) {
    auto sigcodes=cash::tx::combine(sigcodei,sigcodeo);
    auto ret=cash::tx::from_b58(txb58);
    if (unlikely(!ret.first.empty())) {
        return move(ret);
    }

    cash::app::query_accounts_t addresses;
    for (const auto&i:ret.second->get_inputs()) {
        addresses.emplace_back(i.get_address());
    }

    pair<string,wallet::accounts_query_t> r=query_accounts(peer,addresses);
    if (unlikely(!r.first.empty())) {
        ret.first=r.first;
        return ret;
    }
    auto& bases=r.second;
    int n=0;

    for (const auto&i:ret.second->get_inputs()) {
        auto b=bases.find(i.get_address());
        if(unlikely(b==bases.end())) {
            ret.first="Error. Address not found.";
            cerr << "No such address " << i.get_address() << endl;
            return move(ret);
        }
        const cash::app::account_t& src=b->second;
        if (!cash::app::unlock(i.get_address(), n,src.get_locking_program(),
                                             i.get_locking_program_input(),*ret.second)) {
            ret.second->set_locking_program_input(n, 
                          generate_locking_program_input(*ret.second,
                                                         n,sigcodes,
                                                         i.get_address(),
                                                         src.get_locking_program()));
            if (!cash::app::unlock(i.get_address(), n,src.get_locking_program(), 
                                             i.get_locking_program_input(),*ret.second)) {
                ret.second->set_locking_program_input(n, "");  	
                cerr << "warning, cannot unlock account " << i.get_address() << endl;		
            }
       }
       ++n;
    }
    return move(ret);
}

pair<string,unique_ptr<cash::tx>> 
wallet::tx_make_p2pkh(socket::peer_t& peer, 
                     const tx_make_p2pkh_input& i) { 
    pair<string,unique_ptr<cash::tx>> ret;
    ret.second.reset(new cash::tx());
    cash::tx& t=*ret.second;

    auto sigcodes=cash::tx::combine(i.get_sigcode_inputs(), i.get_sigcode_outputs());
    auto s=select_sources(peer, i.get_amount()+i.get_fee());

    if (unlikely(!s.first.empty())) {
        ret.first=s.first;
        return move(ret);
    }

    input_accounts_t& input_accounts=s.second;

    if (input_accounts.empty()) {
        ret.first="Error. Insufficient balance";
        return move(ret);
    }

    if(input_accounts.get_withdraw_amount()!=i.get_amount()+i.get_fee()) {
        ret.first="Error. Inconsistency on amounts.";
        return move(ret);
    }

    for (auto&i:input_accounts) {
        t.add_input(i.m_address, i.m_withdraw_amount);
    }

    t.add_output(i.get_rcpt_addr(), i.get_amount(), cash::p2pkh::locking_program_hash);
    auto fee=t.check();

    if (fee<1){ 
        ostringstream err;
        err << "Failed check. fees are " << fee;
        ret.first=err.str();
        return move(ret);
    }

    if (likely(cash::tx::same_sigmsg_across_inputs(sigcodes))) {
        crypto::ec::sigmsg_hasher_t::value_type h=t.get_hash(0, sigcodes);
        int n=0;
        for (auto&i:t.get_inputs()) {
            t.set_locking_program_input(n, generate_locking_program_input(h,sigcodes,i.get_address(), 
                                                                          input_accounts[n].get_locking_program()));
            ++n;
        }
    }else {
        int n=0;
        for (auto&i:t.get_inputs()) {
            t.set_locking_program_input(n, generate_locking_program_input(t,n,sigcodes,i.get_address(), 
                                                                          input_accounts[n].get_locking_program()));
            ++n;
        }
    }

    if (i.get_sendover()) {
        string r=send(peer,t);
        if (unlikely(!r.empty())) {
            ret.first=r;
        }
    }
    return move(ret);
}

void wallet::tx_make_p2pkh_input::to_stream(ostream& os) const {
    os << m_rcpt_addr << ' ' << m_amount << ' ' << m_fee << ' '
        << m_sigcode_inputs << ' ' << m_sigcode_outputs << ' ' << (m_sendover?'1':'0');
}

wallet::tx_make_p2pkh_input 
wallet::tx_make_p2pkh_input::from_stream(istream& is) {
    
    tx_make_p2pkh_input i;

    hash_t rcpt_addr;
    cash_t amount;
    cash_t fee;
    sigcode_t sigcode_inputs;
    sigcode_t sigcode_outputs;
    bool sendover;

    is >> rcpt_addr;
    is >> amount;
    is >> fee;
    is >> sigcode_inputs;
    is >> sigcode_outputs;
    is >> sendover;

    i.set_rcpt_addr(rcpt_addr);
    i.set_amount(amount);
    i.set_fee(fee);
    i.set_sigcode_inputs(sigcode_inputs);
    i.set_sigcode_outputs(sigcode_outputs);
    i.set_sendover(sendover);

    return move(i);
}
