#ifndef USGOV_7d0dac324059aa8fa70622923a29506fe6d9689a9cc3e3072571557fe53e03f1
#define USGOV_7d0dac324059aa8fa70622923a29506fe6d9689a9cc3e3072571557fe53e03f1

#include <unordered_map>
#include <gov/cash.h>
#include <gov/crypto.h>
#include <gov/cfg.h>
#include <fstream>
#include <unordered_map>

namespace usgov {
using namespace std;

struct wallet: unordered_map<cash::hash_t,crypto::ec::keys>, filesystem::cfg {
	typedef cash::app::account_t account_t;

	struct accounts_query_t:cash::app::accounts_t {
		typedef cash::app::accounts_t b;
		void dump(ostream& os) const;

		blockchain::diff::hash_t parent_block;
	};

	struct input_account_t: cash::app::account_t {
		typedef cash::app::account_t b;
		typedef cash::hash_t hash_t;
		typedef cash::cash_t cash_t;
		input_account_t(const hash_t& address,const b& acc, const cash_t& withdraw_amount);
		void dump(ostream& os) const;
		hash_t address;
		cash_t withdraw_amount;
	};

	struct input_accounts_t:vector<input_account_t> {
		blockchain::diff::hash_t parent_block;
		cash::cash_t get_withdraw_amount() const;
		void dump(ostream& os) const;
	};

	wallet(const string& datapath);
	~wallet();

    string filename() const;

	bool load();
	bool save() const;

	cash::hash_t new_address();
	cash::hash_t add_address(const crypto::ec::keys::priv_t& key);
	const crypto::ec::keys* get_keys(const cash::hash_t& address) const;
	cash::cash_t balance() const;
	void dump_balances(ostream& os) const;
	static accounts_query_t query_accounts(const string&host, uint16_t port, const cash::app::query_accounts_t& addresses);
	void refresh(const string&addr, uint16_t port);
 
	input_accounts_t select_sources(const string&backend_host, uint16_t backend_port, const cash::cash_t& amount);

    string generate_locking_program_input(const crypto::ec::sigmsg_hasher_t::value_type& msg, const cash::tx::sigcodes_t& sigcodes, const cash::hash_t& address, const cash::hash_t& locking_program);
    string generate_locking_program_input(const cash::tx& t, size_t this_index, const cash::tx::sigcodes_t& sigcodes, const cash::hash_t& address, const cash::hash_t& locking_program);
    struct tx_make_p2pkh_input {
        cash::hash_t rcpt_addr;
        cash::cash_t amount;
        cash::cash_t fee;
        cash::tx::sigcode_t sigcode_inputs;
        cash::tx::sigcode_t sigcode_outputs;
        bool sendover;

    };
    pair<string,cash::tx> tx_make_p2pkh(const string&backend_host, uint16_t backend_port, const tx_make_p2pkh_input& i);


	void dump(ostream& os) const;
	accounts_query_t data;
	string datapath;
	mutable bool need_save{false};
};

}

#endif

