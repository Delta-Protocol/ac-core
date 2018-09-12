#include "tx.h"
#include <us/gov/peer.h>
#include "protocol.h"
#include <thread>
#include <chrono>
#include <us/gov/crypto/base58.h>
#include <us/gov/likely.h>
#include <chrono>
#include <iostream>
#include <sys/time.h>

typedef us::gov::cash::tx c;
using namespace us::gov;
using namespace us::gov::cash;
using namespace std;

constexpr array<const char*,c::num_sigcodes> c::sigcodestr;

c::tx() {
    //struct timeval tv;
    //gettimeofday (&tv, NULL);
    //memcpy(&nonce,&tv.tv_usec,sizeof(nonce));
}

c::tx(int) {  //saves cpu
}

void c::dump_sigcodes(ostream&os) {
	os << sigcodestr[sigcode_all] << ' ' << sigcodestr[sigcode_none] << ' ' << sigcodestr[sigcode_this] << ' ';
}

void c::add_input(const hash_t& addr, const cash_t& amount) {
	inputs.push_back(input_t(addr,amount));
}

void c::add_input(const hash_t& addr, const cash_t& amount, const string& locking_program_input) {
	inputs.push_back(input_t(addr,amount,locking_program_input));
}

pair<string,unique_ptr<c>> c::read(istream& is) {
	pair<string,unique_ptr<c>> t=make_pair("", unique_ptr<c>(new c(0)) );
    {
    auto r=t.second->inputs.read(is);
	if (!r.empty()) {
        t.first=r;
//        delete t.second;
//        t.second=0;
        return move(t);       
    }
    }
	auto r=t.second->outputs.read(is);
	if (!r.empty()) {
        t.first=r;
 //       delete t.second;
 //       t.second=0;
        return move(t);       
    }
/*
	is >> t.second.parent_block;
    if (!is.good()) {
        t.first="Unreadable parent_block";
        return move(t);       
    }
*/
	is >> t.second->ts;
    if (!is.good()) {
        t.first="Unreadable timestamp";
//        delete t.second;
//        t.second=0;
        return move(t);       
    }
	return move(t);
}

void c::write_sigmsg(ec::sigmsg_hasher_t& h, size_t this_index, sigcodes_t scs) const {
	inputs.write_sigmsg(h,this_index,sigcode_input(scs));
	outputs.write_sigmsg(h,this_index,sigcode_output(scs));
//	h.write(parent_block);
	h.write(ts);
}

void c::write_pretty(ostream& os) const {
	os << "---transaction---------------" << endl;
	os << "  timestamp: " << ts << endl;
//	os << "  parent_block: " << parent_block << endl;
	os << "  -----" << inputs.size() << " inputs-------" << endl;
	inputs.write_pretty(os);
	os << "  -----" << outputs.size() << " outputs-------" << endl;
	outputs.write_pretty(os);
	os << "-/-transaction---------------" << endl;
}


void c::write(ostream& os) const {
	inputs.write(os);
	outputs.write(os);
//	os << parent_block << ' ' << nonce << ' ';
	os << ts << ' ';
}

string c::to_b58() const {
	ostringstream os;
	write(os);
	return crypto::b58::encode(os.str());
}

pair<string,unique_ptr<c>> c::from_b58(const string& s) {
	string txt=crypto::b58::decode(s);
	istringstream is(txt);
//    pair<string,c*> r;
//	auto ret=read(is);
	return read(is);
//    if (unlikely(!ret.first.empty())) {
        //r.first=x.first;
 //       return move(r);
   // }
    //r.second=x.second;
    //return move(r);
}

void c::inputs_t::write_sigmsg(ec::sigmsg_hasher_t& h, size_t this_index, sigcode_t sc) const {
	if (likely(sc==sigcode_all)) {
		for (auto& i:*this) i.write_sigmsg(h);
	}
	else { //sigcode_none and sigcode_this 
		(*this)[this_index].write_sigmsg(h);
	}
}

void c::inputs_t::write_pretty(ostream& os) const {
	int n=0;
	for (auto& i:*this) {
		os << "  --input #" << n << endl;
		i.write_pretty(os);
		++n;
	}
}

void c::inputs_t::write(ostream& os) const {
	os << size() << ' ';
	for (auto& i:*this) i.write(os);
}

string c::inputs_t::read(istream& is) {
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

void c::outputs_t::write_sigmsg(ec::sigmsg_hasher_t& h, size_t this_index, sigcode_t sc) const {
	if (likely(sc==sigcode_all)) {
		for (auto& i:*this) i.write_sigmsg(h);
	}
	else if (sc==sigcode_this) {
		if (unlikely(this_index>=size())) return;
		(*this)[this_index].write_sigmsg(h);
	}
}
void c::outputs_t::write(ostream& os) const {
	os << size() << ' ';
	for (auto& i:*this) i.write(os);
}
void c::outputs_t::write_pretty(ostream& os) const {
	int n=0;
	for (auto& i:*this) {
		os << "  --output #" << n << endl;
		i.write_pretty(os);
		++n;
	}
}

string c::outputs_t::read(istream& is) {
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

void c::input_t::write_sigmsg(ec::sigmsg_hasher_t& h) const {
	h.write(address);

//	h.write(reinterpret_cast<const unsigned char*>(&spend_code), sizeof(spend_code_t));
//	h.write(reinterpret_cast<const unsigned char*>(&prev_balance), sizeof(prev_balance));
	h.write(reinterpret_cast<const unsigned char*>(&amount), sizeof(amount));
}
void c::input_t::write(ostream& os) const {
	os << address << ' ' /*<< prev_balance << ' '*/ << amount << ' ' << crypto::b58::encode(locking_program_input.empty()?"-":locking_program_input) << ' ';
}

void c::input_t::write_pretty(ostream& os) const {
	os << "    address: " << address << endl;
//	os << "    balance: " << prev_balance << "; withdraw " << amount << "; final balance: " << (prev_balance-amount) << endl;
	os << "    withdraw: " << amount << endl;
	os << "    unlock with: " << locking_program_input << endl;
}

void c::output_t::write_sigmsg(ec::sigmsg_hasher_t& h) const {
//	os << address << ' ' << amount << ' ' << locking_program << ' ';
	h.write(address);
	h.write(reinterpret_cast<const unsigned char*>(&amount), sizeof(amount));
	h.write(locking_program);
}
void c::output_t::write(ostream& os) const {
	os << address << ' ' << amount << ' ' << locking_program << ' ';
}
void c::output_t::write_pretty(ostream& os) const {
	os << "    address: " << address << endl;
	os << "    amount: " << amount << endl;
	os << "    locking_program: " << locking_program << endl;
}

pair<string,c::input_t> c::input_t::read(istream& is) {
	pair<string,input_t> o0;
    auto& o=o0.second;
	is >> o.address;
//	is >> o.prev_balance;
	is >> o.amount;
	string locking_program_input_b58;
	is >> locking_program_input_b58;
	o.locking_program_input=crypto::b58::decode(locking_program_input_b58);
	if (o.locking_program_input=="-") o.locking_program_input.clear();
    if (unlikely(!is.good())) {
        o0.first="Error reading transaction input.";
    }
	return move(o0);
}

pair<string,c::output_t> c::output_t::read(istream& is) {
	pair<string,output_t> o0;
    auto& o=o0.second;
	is >> o.address;
	is >> o.amount;
	is >> o.locking_program;
    if (unlikely(!is.good())) {
        o0.first="Error reading transaction output.";
    }
	return move(o0);
}

void c::add_output(const hash_t& addr, const cash_t& amount, const hash_t& locking_program) {
	outputs.push_back(output_t(addr,amount,locking_program));
}

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

ec::sigmsg_hasher_t::value_type c::get_hash(const size_t& this_index, sigcodes_t sc) const {
	ec::sigmsg_hasher_t h;
	write_sigmsg(h,this_index,sc);
	ec::sigmsg_hasher_t::value_type v;
	h.finalize(v);
//cout << "sighash for index " << this_index << " sigcodes " << (int)sc << " " << v << endl;
	return move(v);
}

datagram* c::get_datagram() const {
	return new socket::datagram(protocol::cash_tx,to_b58());
}

cash_t c::check() const {
	cash_t sum_inputs{0};
	cash_t sum_outputs{0};
	for (auto& i:inputs) {
//cout << i.amount << endl;
		if (i.amount<=0) return -1; //prevent infinite number of inputs attack, i.e. all inputs carry a cost
		sum_inputs+=i.amount;
	}
//cout << "O" << endl;
	for (auto& i:outputs) {
//cout << i.amount << endl;
		if (i.amount<=0) return -1; //prevent infinite number of outputs attack, i.e. all outputs carry a cost
		sum_outputs+=i.amount;
	}
	return sum_inputs-sum_outputs;
}


