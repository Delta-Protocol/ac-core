#include "tx.h"

#include <iostream>
#include <sys/time.h>
#include <thread>
#include <chrono>

#include <us/gov/peer.h>
#include <us/gov/crypto/base58.h>
#include <us/gov/likely.h>
#include "protocol.h"


using namespace us::gov;
using namespace us::gov::cash;
using namespace std;

constexpr array<const char*,tx::num_sigcodes> tx::m_sigcodestr;

void tx::dump_sigcodes(ostream&os) {
    os << m_sigcodestr[sigcode_all] << ' ' 
       << m_sigcodestr[sigcode_none] << ' ' 
       << m_sigcodestr[sigcode_this] << ' ';
}

void tx::add_input(const hash_t& addr, const cash_t& amount) {
    m_inputs.push_back(input_t(addr,amount));
}

void tx::add_input(const hash_t& addr, const cash_t& amount, 
                  const string& locking_program_input) {
    m_inputs.push_back(input_t(addr,amount,locking_program_input));
}

pair<string,unique_ptr<tx>> 
tx::read(istream& is) {
    pair<string,unique_ptr<tx>> t=make_pair("", unique_ptr<tx>(new tx(0)) );
    {
        auto r=t.second->m_inputs.read(is);
	if (!r.empty()) {
            t.first=r;
            return move(t);       
        }
    }
    auto r=t.second->m_outputs.read(is);
    if (!r.empty()) {
        t.first=r;
        return move(t);       
    }
    uint64_t ts;
    is >>ts;
    t.second->set_ts(ts);
    if (!is.good()) {
        t.first="Unreadable timestamp";
        return move(t);       
    }
    return move(t);
}

void tx::write_sigmsg(ec::sigmsg_hasher_t& h, 
                     size_t this_index, sigcodes_t scs) const {
    m_inputs.write_sigmsg(h,this_index,sigcode_input(scs));
    m_outputs.write_sigmsg(h,this_index,sigcode_output(scs));
    h.write(get_ts());
}

void tx::write_pretty(ostream& os) const {
    os << "---transaction---------------" << endl;
    os << "  timestamp: " << get_ts() << endl;
    os << "  -----" << m_inputs.size() << " inputs-------" << endl;
    m_inputs.write_pretty(os);
    os << "  -----" << m_outputs.size() << " outputs-------" << endl;
    m_outputs.write_pretty(os);
    os << "-/-transaction---------------" << endl;
}


void tx::write(ostream& os) const {
    m_inputs.write(os);
    m_outputs.write(os);
    os << get_ts() << ' ';
}

string tx::to_b58() const {
    ostringstream os;
    write(os);
    return crypto::b58::encode(os.str());
}

pair<string,unique_ptr<tx>> tx::from_b58(const string& s) {
    string txt=crypto::b58::decode(s);
    istringstream is(txt);
    return read(is);
}

void tx::add_output(const hash_t& addr, 
                   const cash_t& amount, 
                   const hash_t& locking_program) {
    m_outputs.push_back(output_t(addr,amount,locking_program));
}

datagram* tx::get_datagram() const {
    return new socket::datagram(protocol::cash_tx,to_b58());
}

cash_t tx::check() const {
    cash_t sum_inputs{0};
    cash_t sum_outputs{0};
    for (auto& i:m_inputs) {
        if (i.get_amount()<=0) 
            return -1;
            sum_inputs+=i.get_amount();
    }
    for (auto& i:m_outputs) {
        if (i.get_amount()<=0) 
            return -1;
            sum_outputs+=i.get_amount();
    }
    return sum_inputs-sum_outputs;
}

void tx::inputs_t::write_sigmsg(ec::sigmsg_hasher_t& h, 
                               size_t this_index, sigcode_t sc) const {
    if (likely(sc==sigcode_all)) {
        for (auto& i:*this) i.write_sigmsg(h);
    }else {  
        (*this)[this_index].write_sigmsg(h);
    }
}

void tx::inputs_t::write_pretty(ostream& os) const {
    int n=0;
    for (auto& i:*this) {
        os << "  --input #" << n << endl;
        i.write_pretty(os);
        ++n;
    }
}

void tx::inputs_t::write(ostream& os) const {
    os << size() << ' ';
    for (auto& i:*this) 
        i.write(os);
}

string tx::inputs_t::read(istream& is) {
    size_t sz;
    is >> sz;
    if (unlikely(sz>100)) { //max number of outputs TODO
        return "Error. There is a limit of 100 inputs.";
    }
    reserve(sz);
    for (size_t i=0; i<sz; ++i) {
        auto x=input_t::read(is);
        if (unlikely(!x.first.empty())) {
            return x.first;
        }
        push_back(x.second);
    }
    return "";
}

void tx::outputs_t::write_sigmsg(ec::sigmsg_hasher_t& h, 
                                size_t this_index, sigcode_t sc) const {
    if (likely(sc==sigcode_all)) {
        for (auto& i:*this) i.write_sigmsg(h);
    }else if (sc==sigcode_this) {
        if (unlikely(this_index>=size())) 
            return;
        (*this)[this_index].write_sigmsg(h);
    }
}

void tx::outputs_t::write(ostream& os) const {
    os << size() << ' ';
    for (auto& i:*this) 
        i.write(os);
}
void tx::outputs_t::write_pretty(ostream& os) const {
    int n=0;
    for (auto& i:*this) {
        os << "  --output #" << n << endl;
        i.write_pretty(os);
        ++n;
    }
}

string tx::outputs_t::read(istream& is) {
    size_t sz;
    is >> sz;
    if (unlikely(sz>100)) { //max number of outputs
        return "Error. There is a limit of 100 outputs.";
    }
    reserve(sz);
    for (size_t i=0; i<sz; ++i) {
        auto x=output_t::read(is);
        if (unlikely(!x.first.empty())) {
            return x.first;
        }
        push_back(x.second);
    }
    return "";
}

void tx::input_t::write_sigmsg(ec::sigmsg_hasher_t& h) const {
    h.write(m_address);
    h.write(reinterpret_cast<const unsigned char*>(&m_amount), sizeof(m_amount));
}
void tx::input_t::write(ostream& os) const {
    os << m_address << ' ' << m_amount << ' ' 
       << crypto::b58::encode(m_locking_program_input.empty()?"-":m_locking_program_input) << ' ';
}

void tx::input_t::write_pretty(ostream& os) const {
    os << "    address: "     << m_address << endl;
    os << "    withdraw: "    << m_amount  << endl;
    os << "    unlock with: " << m_locking_program_input << endl;
}

void tx::output_t::write_sigmsg(ec::sigmsg_hasher_t& h) const {
    h.write(m_address);
    h.write(reinterpret_cast<const unsigned char*>(&m_amount), sizeof(m_amount));
    h.write(m_locking_program);
}

void tx::output_t::write(ostream& os) const {
    os << m_address << ' '
       << m_amount << ' ' << m_locking_program << ' ';
}

void tx::output_t::write_pretty(ostream& os) const {
    os << "    address: " << m_address << endl;
    os << "    amount: "  << m_amount  << endl;
    os << "    locking_program: " << m_locking_program << endl;
}

pair<string,tx::input_t> 
tx::input_t::read(istream& is) {
    pair<string,input_t> o0;
    auto& o=o0.second;
    is >> o.m_address;
    is >> o.m_amount;
    string locking_program_input_b58;
    is >> locking_program_input_b58;
    
    o.m_locking_program_input=crypto::b58::decode(locking_program_input_b58);
    
    if (o.m_locking_program_input=="-") 
        o.m_locking_program_input.clear();
    if (unlikely(!is.good())) {
        o0.first="Error reading transaction input.";
    }
    return move(o0);
}

pair<string,tx::output_t> tx::output_t::read(istream& is) {
    pair<string,output_t> o0;
    auto& o=o0.second;
    is >> o.m_address;
    is >> o.m_amount;
    is >> o.m_locking_program;
    
    if (unlikely(!is.good())) {
        o0.first="Error reading transaction output.";
    }
    return move(o0);
}

ec::sigmsg_hasher_t::value_type 
tx::get_hash(const size_t& this_index, sigcodes_t sc) const {
    ec::sigmsg_hasher_t h;
    write_sigmsg(h,this_index,sc);
    ec::sigmsg_hasher_t::value_type v;
    h.finalize(v);
    return move(v);
}


