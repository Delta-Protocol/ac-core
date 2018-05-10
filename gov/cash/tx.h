#ifndef USGOV_33de4fece6646b4a064a30507e7a1a35c3b5bcd4dc9c1c87059904e7ac05e335
#define USGOV_33de4fece6646b4a064a30507e7a1a35c3b5bcd4dc9c1c87059904e7ac05e335

#include <gov/auth.h>
#include <gov/blockchain.h>

#include <vector>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <gov/signal_handler.h>
#include <gov/crypto/crypto.h>
//#include <blockchain/block.h>
#include "protocol.h"
#include <chrono>
#include <cassert>
#include <map>

namespace us { namespace gov {
namespace cash {
	using namespace std;
	using socket::datagram;
//	using blockchain::signature;
	using blockchain::local_deltas;
	using crypto::ripemd160;
	using crypto::ec;
	using blockchain::peer_t;

	typedef int64_t cash_t;
	typedef crypto::ec::keys keys;
	typedef keys::pub_t pubkey_t;

	typedef ripemd160 hasher_t;
	typedef hasher_t::value_type hash_t;

	struct tx {
		enum sigcode_t {
			sigcode_all = 0,
			sigcode_none = 1,
			sigcode_this = 2,
			num_sigcodes
		};
		constexpr static array<const char*,num_sigcodes> sigcodestr={"all","none","this"};
		static void dump_sigcodes(ostream&os);

		struct input_t {
			input_t() {}
			input_t(const hash_t& address, const cash_t& prev_balance, const cash_t& amount): address(address), prev_balance(prev_balance), amount(amount) {
			}
			input_t(const hash_t& address, const cash_t& prev_balance, const cash_t& amount, const string& locking_program_input): address(address), prev_balance(prev_balance), amount(amount), locking_program_input(locking_program_input) {
			}
			void write_sigmsg(ec::sigmsg_hasher_t&) const;
			void write(ostream&) const;
			void write_pretty(ostream& os) const;
			static input_t read(istream&);


			hash_t address;
			//spend_code_t spend_code;
			cash_t prev_balance;
			cash_t amount;//rename->delta_cash;

			string locking_program_input;

			//bool verify() const;
			//string message_to_sign() const;
		};
		struct output_t {
			output_t() {}
			output_t(const hash_t& address, const cash_t& amount, const hash_t& locking_program): address(address), amount(amount), locking_program(locking_program) {
			}
			void write_sigmsg(ec::sigmsg_hasher_t&) const;
			void write(ostream&) const;
			void write_pretty(ostream& os) const;
			static output_t read(istream&);

			hash_t address;
			cash_t amount;
			hash_t locking_program;
		};
		typedef unsigned char sigcodes_t;
		struct inputs_t:vector<input_t> {
			void write_sigmsg(ec::sigmsg_hasher_t&, size_t input_index, sigcode_t sh) const;
			void write(ostream&) const;
			void write_pretty(ostream& os) const;
			void read(istream&);
		};

		struct outputs_t:vector<output_t> {
			void write_sigmsg(ec::sigmsg_hasher_t&, size_t input_index, sigcode_t sh) const;
			void write(ostream&) const;
			void write_pretty(ostream& os) const;
			void read(istream&);
		};
		static inline sigcodes_t combine(sigcode_t i, sigcode_t o) { return (i<<4)|o; }
		static inline sigcode_t sigcode_input(sigcodes_t i) { return (sigcode_t)(i>>4); }
		static inline sigcode_t sigcode_output(sigcodes_t i) { return (sigcode_t)(i&0x0f); }
		static bool same_sigmsg_across_inputs(sigcodes_t i) { return sigcode_input(i)==sigcode_this || sigcode_output(i)==sigcode_this; }


		bool add_input(const hash_t& addr, const cash_t& prev_balance, const cash_t& amount/*, const spend_code_t& spend_code*/);
		bool add_input(const hash_t& addr, const cash_t& prev_balance, const cash_t& amount, const string& locking_program_input);
		bool add_output(const hash_t& addr, const cash_t& amount, const hash_t& locking_program);

		cash_t check() const; //return fees

		ec::sigmsg_hasher_t::value_type get_hash(const size_t& this_index, sigcodes_t) const;
		static tx read(istream&);

		void write(ostream&) const;
		void write_sigmsg(ec::sigmsg_hasher_t&, size_t input_index, sigcodes_t sh) const;
		void write_pretty(ostream& os) const;
		string to_b58() const;
		static tx from_b58(const string&);

		datagram* get_datagram() const;

		inputs_t inputs;
		outputs_t outputs;
		blockchain::diff::hash_t parent_block;

	};

}

static ostream& operator << (ostream&os, const cash::tx& t) {
	os << t.to_b58() << endl;
	return os;
}

static ostream& operator << (ostream&os, const cash::tx::sigcodes_t& x) {
	os << (int)x;
	return os;
}

static istream& operator >> (istream&is, cash::tx::sigcodes_t& t) {
	int x;
	is >> x;
	t=(cash::tx::sigcodes_t)x;
	return is;
}

static ostream& operator << (ostream&os, const cash::tx::sigcode_t& x) {
	os << (int)x;
	return os;
}

static istream& operator >> (istream&is, cash::tx::sigcode_t& t) {
	int x;
	is >> x;
	t=(cash::tx::sigcode_t)x;
	return is;
}

}}

#endif

