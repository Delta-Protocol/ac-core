#include <gov/cash.h>
#include <gov/crypto.h>
#include <string>
#include <chrono>
#include <thread>
#include <gov/cash/locking_programs/p2pkh.h>
#include <gov/signal_handler.h>
#include <gov/socket/datagram.h>
#include "wallet.h"
#include "daemon.h"
#include "args.h"

using namespace usgov;
using namespace std;


void sig_handler(int s) {
    cout << "main: Caught signal " << s << endl;
    signal_handler::_this.finish();
    signal(SIGINT,SIG_DFL);
    signal(SIGTERM,SIG_DFL);
}

namespace usgov {
	template<> cash::tx::sigcode_t convert(const string& s) {
		if (s==cash::tx::sigcodestr[cash::tx::sigcode_all]) return cash::tx::sigcode_all;
		if (s==cash::tx::sigcodestr[cash::tx::sigcode_none]) return cash::tx::sigcode_none;
		if (s==cash::tx::sigcodestr[cash::tx::sigcode_this]) return cash::tx::sigcode_this;
		return cash::tx::sigcode_all;
	}
	template<> crypto::ec::keys::priv_t convert(const string& s) {
		return crypto::ec::keys::priv_t::from_b58(s);
	}

}

struct params {
	params() {
	    const char* env_p = std::getenv("HOME");
	    if (!env_p) {
		    cerr << "No $HOME env var defined" << endl;
		    exit(1);
	    }
	    homedir=env_p;
	    homedir+="/.us";
	}
    void dump(ostream& os) const {
        os << "home: " << homedir << endl;
        os << "backend: " << backend_host << ":" << backend_port << endl;
        os << "walletd: " << walletd_host << ":" << walletd_port << endl;
    }
	bool daemon{false};
    string homedir;
    bool offline{false};
	string backend_host{"localhost"}; uint16_t backend_port{16672};
    string walletd_host{"localhost"}; uint16_t walletd_port{16673};
};


void help(const params& p) {
    cout << "us.gov wallet" << endl;
    cout << "usage:" << endl;
    cout << " wallet [options] command" << endl;
    cout << endl;
    cout << "options are:" << endl;
	cout << " -home <homedir>   homedir. [" << p.homedir << "]" << endl;
	cout << " -local    Load data from local homedir instead of connecting to a wallet daemon. [" << boolalpha << p.offline << "]" << endl;
if (p.offline) {
	cout << " backend connector:" << endl;
	cout << " -bhost <address>  backend host. [" << p.backend_host << "]" << endl;
	cout << " -bp <port>  backend port. [" << p.backend_port << "]" << endl;
}
else {
	cout << " walletd connector:" << endl;
	cout << " -whost <address>  walletd address. [" << p.walletd_host << "]" << endl;
	cout << " -wp <port>  walletd port. [" << p.walletd_port << "]" << endl;
}
    cout << endl;
    cout << "commands are:" << endl;
	cout << " balance [0|1]          Displays the spendable amount." << endl;
	cout << " address new            Generates a new key-pair, adds the private key to the wallet and prints its asociated address." << endl;
	cout << " address add <privkey>  Imports a given private key in the wallet" << endl;
	cout << " dump                   Lists the keys/addresses managed by wallet" << endl;
	cout << " gen_keys               Generates a key pair without adding them to the wallet." << endl;
	cout << " priv_key <private key> Gives information about the given private key." << endl;
	cout << " tx base                Reports the current parent block for new transactions" << endl;
	cout << " tx make <parent-block> <src account> <prev balance> <withdraw amount> <dest account> <deposit amount> <locking program hash>" << endl;
	cout << " tx make_p2pkh <dest account> <amount> <fee> <sigcode_inputs=all> <sigcode_outputs=all> [<send>]" << endl;
	cout << " tx decode <tx_b58>" << endl;
	cout << " tx check <tx_b58>" << endl;
	cout << " tx send <tx_b58>" << endl;
	cout << " tx sign <tx_b58> <sigcode_inputs> <sigcode_outputs>" << endl;
	cout << "    sigcodes: "; cash::tx::dump_sigcodes(cout); cout << endl;
	cout << " daemon                 Run wallet daemon on port " << p.walletd_port << endl;
}


string generate_locking_program_input(const params& p, const crypto::ec::sigmsg_hasher_t::value_type& msg, const cash::tx::sigcodes_t& sigcodes, const cash::hash_t& address, const cash::hash_t& locking_program) {
	if (likely(locking_program<cash::min_locking_program)) {
		if (unlikely(locking_program==0)) {
			return "";
		}
		else if (locking_program==1) {
			wallet my_wallet(p.homedir);
			const crypto::ec::keys* k=my_wallet.get_keys(address);
			if (k==0) return "";
			return cash::p2pkh::create_input(msg, sigcodes, k->priv);
		}
	}
	return "";
}

string generate_locking_program_input(const params& p, const cash::tx& t, size_t this_index, const cash::tx::sigcodes_t& sigcodes, const cash::hash_t& address, const cash::hash_t& locking_program) {
	if (likely(locking_program<cash::min_locking_program)) {
		if (unlikely(locking_program==0)) {
			return "";
		}
		else if (locking_program==1) {
			wallet my_wallet(p.homedir);
			const crypto::ec::keys* k=my_wallet.get_keys(address);
			if (k==0) return "";
			return cash::p2pkh::create_input(t,this_index, sigcodes, k->priv);
		}
	}
	return "";
}


void send(const cash::tx& t, const params& p) {
	auto fee=t.check();
	if (fee<=0) {
		cerr << "Individual inputs and fees must be positive." << endl;
		exit(1);
	}
	socket::peer_t cli;
	if (!cli.connect(p.backend_host,p.backend_port,true)) {
		cerr << "wallet: unable to connect to " << p.backend_host << ":" << p.backend_port << endl;
		exit(1);
	}
	cli.send(t.get_datagram());
}

using datagram=socket::datagram;

void run_daemon(const params& p) {
	signal(SIGINT,sig_handler);
	signal(SIGTERM,sig_handler);
	signal(SIGPIPE, SIG_IGN);

	wallet_daemon d(p.walletd_port, p.homedir, p.backend_host, p.backend_port);
	d.run();
}

#include "protocol.h"


string parse_options(args_t& args, params& p) {
    string cmd;
    while(true) {
    	cmd=args.next<string>();
        if (cmd=="-wp") {
        	p.walletd_port=args.next<int>();
        }
        else if (cmd=="-whost") {
        	p.walletd_host=args.next<string>();
        }
        else if (cmd=="-bp") {
        	p.backend_port=args.next<int>();
        }
        else if (cmd=="-bhost") {
        	p.backend_host=args.next<string>();
        }
        else if (cmd=="-local") {
        	p.offline=true;
        }
        else if (cmd=="-home") {
        	p.homedir=args.next<string>();
        }
        else {
            break;
        }
    }
    return cmd;        
}

void tx_make(args_t& args, const params& p) {
	cash::tx t;
	t.parent_block=args.next<blockchain::diff::hash_t>();

	auto src=args.next<cash::hash_t>();
	auto prev_balance=args.next<cash::cash_t>();
	auto withdraw_amount=args.next<cash::cash_t>();

	auto dst=args.next<cash::hash_t>();
	auto deposit_amount=args.next<cash::cash_t>();
	auto locking_program=args.next<cash::hash_t>();

	t.add_input(src, prev_balance, withdraw_amount);
	t.add_output(dst, deposit_amount, locking_program);
	if (!t.check()) {
		cerr << "Error" << endl;
		exit(1);
	}

	cout << endl;
	cout << t << endl;
	//DECODED:
	cout << "decoded:" << endl;
	t.write(cout);	
	cout << endl;
}


void tx_make_p2pkh(args_t& args, const params& p) {
		auto dstaddr=args.next<cash::hash_t>();
		auto amount=args.next<cash::cash_t>();
		auto fee=args.next<cash::cash_t>();
		auto sigcode_inputs=args.next<cash::tx::sigcode_t>(cash::tx::sigcode_all);
		auto sigcode_outputs=args.next<cash::tx::sigcode_t>(cash::tx::sigcode_all);
		auto sigcodes=cash::tx::combine(sigcode_inputs,sigcode_outputs);
		bool sendover=args.next<string>("nopes")=="send";

		wallet my_wallet(p.homedir);
		wallet::input_accounts_t input_accounts=my_wallet.select_sources(p.backend_host, p.backend_port, amount+fee);
		if (input_accounts.empty()) {
			cerr << "no inputs available." << endl;
			exit(1);
		}
		assert(input_accounts.get_withdraw_amount()==amount+fee);

		cash::tx t;
		t.parent_block=input_accounts.parent_block;
		t.inputs.reserve(input_accounts.size());
		for (auto&i:input_accounts) {
			t.add_input(i.address, i.balance, i.withdraw_amount);
		}
		t.add_output(dstaddr, amount, cash::p2pkh::locking_program_hash);
		if (!t.check()) {
			cerr << "Error" << endl;
			exit(1);
		}
		if (likely(cash::tx::same_sigmsg_across_inputs(sigcodes))) {
			crypto::ec::sigmsg_hasher_t::value_type h=t.get_hash(0, sigcodes);
			int n=0;
			for (auto&i:t.inputs) {
				i.locking_program_input=generate_locking_program_input(p,h,sigcodes,i.address, input_accounts[n].locking_program);
				++n;
			}
		}
		else { //sigmsg optimization
			int n=0;
			for (auto&i:t.inputs) {
				i.locking_program_input=generate_locking_program_input(p,t,n,sigcodes,i.address, input_accounts[n].locking_program);
				++n;
			}
		}
//		cout << endl;
		cout << t << endl;

		if (sendover) {
			send(t,p);
			cout << "sent." << endl;
		}
}

void tx_sign(args_t& args, const params& p) {
	auto b58=args.next<string>();
	auto sigcodei=args.next<cash::tx::sigcode_t>();
	auto sigcodeo=args.next<cash::tx::sigcode_t>();
	auto sigcodes=cash::tx::combine(sigcodei,sigcodeo);

	cash::tx t=cash::tx::from_b58(b58);

	cash::app::query_accounts_t addresses;
	for (auto&i:t.inputs) {
		addresses.emplace_back(i.address);
	}
	wallet::accounts_query_t bases=wallet::query_accounts(p.backend_host, p.backend_port, addresses);
	bases.dump(cout);
	int n=0;
	for (auto&i:t.inputs) {
		auto b=bases.find(i.address);
		if(b==bases.end()) {
			cerr << "No such address " << i.address << endl;	
			exit(1);
		}
		const cash::app::account_t& src=b->second;
		if (!cash::app::unlock(i.address, n,src.locking_program,i.locking_program_input,t)) {
			i.locking_program_input=generate_locking_program_input(p,t,n,sigcodes,i.address, src.locking_program);
			if (!cash::app::unlock(i.address, n,src.locking_program,i.locking_program_input,t)) {	
				i.locking_program_input="";
				cout << "cannot unlock account " << i.address << endl;
			}
		}	
		else {
			cout << "unlocked! " << i.address << endl;
		}
		++n;
	}
}

void tx(args_t& args, const params& p) {
	string command=args.next<string>();
	if (command=="make") {
        tx_make(args,p);
	}
	else if (command=="make_p2pkh") {
        tx_make_p2pkh(args,p);
	}
	else if (command=="sign") { //access backend
        tx_sign(args,p);
	}
	else if (command=="decode") {
		auto b58=args.next<string>();
		cash::tx t=cash::tx::from_b58(b58);
		t.write_pretty(cout);
	}
	else if (command=="check") {
		auto b58=args.next<string>();
		cash::tx t=cash::tx::from_b58(b58);
		auto fee=t.check();
		if (fee<=0) {
			cerr << "Individual inputs and fees must be positive." << endl;
			exit(1);
		}
	}
	else if (command=="send") {
		auto b58=args.next<string>();
		cash::tx t=cash::tx::from_b58(b58);
		send(t,p);
		cout << "sent" << endl;
		
	}
	else if (command=="base") {
		cash::app::query_accounts_t addresses;
		addresses.add("2vVN9EUdmZ5ypMe84JrQqwExMRjn");
		addresses.add("mok8mKgni4Bjv1z6NsE3JUDG6FG");
		wallet::accounts_query_t bases=wallet::query_accounts(p.backend_host, p.backend_port, addresses);
		bases.dump(cout);
	}
	else {
		help(p);
		exit(1);
	}
}

int main(int argc, char** argv) {
	args_t args(argc,argv);
	params p;
	string command=parse_options(args,p);

	if (command=="tx") {
        tx(args,p);
	}
	else if (command=="priv_key") {
		auto privkey=args.next<crypto::ec::keys::priv_t>();
		if (!crypto::ec::keys::verify(privkey)) {
			cout << "The private key is incorrect." << endl;
		}
		auto pub=crypto::ec::keys::get_pubkey(privkey);
		cout << "Public key: " << pub << endl;
		cout << "Public key hash: " << pub.compute_hash() << endl;
	}
	else if (command=="address") {
		command=args.next<string>();
		if (command=="new") {
			wallet my_wallet(p.homedir);
			auto a=my_wallet.new_address();
			cout << "Address: " << a << endl;
		}
		else if (command=="add") {
			crypto::ec::keys::priv_t k=args.next<crypto::ec::keys::priv_t>();
			wallet my_wallet(p.homedir);
			auto a=my_wallet.add_address(k);
			cout << "Address: " << a << endl;
		}
		else {
			help(p);
			exit(1);
		}
	}
	else if (command=="dump") {
		wallet my_wallet(p.homedir);
		my_wallet.dump(cout);
	} 
	else if (command=="balance") {
		bool detailed=args.next<bool>(false);
		if (p.offline) {
			wallet my_wallet(p.homedir);
            if (my_wallet.empty()) {
                cerr << "wallet " << my_wallet.filename() << " doesn't contain keys." << endl;
                exit(1);
            }
			my_wallet.refresh(p.backend_host,p.backend_port);
			if (detailed) {
				my_wallet.dump_balances(cout);
			}
			else {
				cout << my_wallet.balance() << endl;
			}
		}
		else {
			datagram* q=new datagram(protocol::wallet::balance_query,"");
			socket::datagram* response=socket::peer_t::send_recv(p.walletd_host, p.walletd_port, q);
			if (response) {
			cout << response->parse_string() << endl;
			delete response;
			}
			else {
				cerr << "ERROR" << endl;
			}
		}
	}
	else if (command=="gen_keys") {
		crypto::ec::keys k=crypto::ec::keys::generate();
		cout << "Private key: " << k.priv.to_b58() << endl;
		cout << "Public key: " << k.pub.to_b58() << endl;
		cout << "Address: " << k.pub.compute_hash() << endl;
		return 0;

	}
	else if (command=="daemon") {
		run_daemon(p);
	}
	else {
		help(p);
		exit(1);
	}
	return 0;
}

