#include "tx.h"
#include <us/gov/peer.h>
#include "protocol.h"
#include <thread>
#include <chrono>
#include <us/gov/crypto/base58.h>
#include <us/gov/likely.h>

typedef us::gov::cash::tx c;
using namespace us::gov;
using namespace us::gov::cash;
using namespace std;

void c::dump_sigcodes(ostream&os) {
	os << sigcodestr[sigcode_all] << ' ' << sigcodestr[sigcode_none] << ' ' << sigcodestr[sigcode_this] << ' ';
}

bool c::add_input(const hash_t& addr, const cash_t& prev_balance, const cash_t& amount/*, const spend_code_t& spend_code*/) {
	inputs.push_back(input_t(addr/*,spend_code*/,prev_balance,amount));
	return true;
}


bool c::add_input(const hash_t& addr, const cash_t& prev_balance, const cash_t& amount, const string& locking_program_input) {
	inputs.push_back(input_t(addr,prev_balance,amount,locking_program_input));
	return true;
}

c c::read(istream& is) {
	tx t;
	t.inputs.read(is);
	t.outputs.read(is);
	is >> t.parent_block;
	return move(t);
}

void c::write_sigmsg(ec::sigmsg_hasher_t& h, size_t this_index, sigcodes_t scs) const {
	inputs.write_sigmsg(h,this_index,sigcode_input(scs));
	outputs.write_sigmsg(h,this_index,sigcode_output(scs));
	h.write(parent_block);
}

void c::write_pretty(ostream& os) const {
	os << "---transaction---------------" << endl;
	os << "  parnet_block: " << parent_block << endl;
	os << "  -----" << inputs.size() << " inputs-------" << endl;
	inputs.write_pretty(os);
	os << "  -----" << outputs.size() << " outputs-------" << endl;
	outputs.write_pretty(os);
	os << "-/-transaction---------------" << endl;
}

void c::write(ostream& os) const {
	inputs.write(os);
	outputs.write(os);
	os << parent_block << ' ';
}

string c::to_b58() const {
	ostringstream os;
	write(os);
	return crypto::b58::encode(os.str());
}

c c::from_b58(const string& s) {
	string txt=crypto::b58::decode(s);
	istringstream is(txt);
	return read(is);
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
void c::inputs_t::read(istream& is) {
	size_t sz;
	is >> sz;
	reserve(sz);
	for (size_t i=0; i<sz; ++i) {
		push_back(input_t::read(is));
	}
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

void c::outputs_t::read(istream& is) {
	size_t sz;
	is >> sz;
	reserve(sz);
	for (size_t i=0; i<sz; ++i) {
		push_back(output_t::read(is));
	}
}

void c::input_t::write_sigmsg(ec::sigmsg_hasher_t& h) const {
	h.write(address);

//	h.write(reinterpret_cast<const unsigned char*>(&spend_code), sizeof(spend_code_t));
	h.write(reinterpret_cast<const unsigned char*>(&prev_balance), sizeof(prev_balance));
	h.write(reinterpret_cast<const unsigned char*>(&amount), sizeof(amount));
}
void c::input_t::write(ostream& os) const {
	os << address << ' ' << prev_balance << ' ' << amount << ' ' << crypto::b58::encode(locking_program_input.empty()?"-":locking_program_input) << ' ';
}

void c::input_t::write_pretty(ostream& os) const {
	os << "    address: " << address << endl;
	os << "    balance: " << prev_balance << "; withdraw " << amount << "; final balance: " << (prev_balance-amount) << endl;
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

c::input_t c::input_t::read(istream& is) {
	input_t o;
	is >> o.address;
//	is >> o.spend_code;
	is >> o.prev_balance;
	is >> o.amount;
	string locking_program_input_b58;
	is >> locking_program_input_b58;
	o.locking_program_input=crypto::b58::decode(locking_program_input_b58);
	if (o.locking_program_input=="-") o.locking_program_input.clear();
	return move(o);
}

c::output_t c::output_t::read(istream& is) {
	output_t o;
	is >> o.address;
	is >> o.amount;
	is >> o.locking_program;
	return move(o);
}


bool c::add_output(const hash_t& addr, const cash_t& amount, const hash_t& locking_program) {
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


