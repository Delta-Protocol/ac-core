#include <gov/cash.h>
#include <gov/crypto.h>
#include <string>
#include <chrono>
#include <thread>
#include <gov/cash/locking_programs/p2pkh.h>
#include <gov/cash/tx.h>
#include <gov/signal_handler.h>
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


void help(const params& p, ostream& os=cout) {
    os << "us.gov wallet" << endl;
    os << "usage:" << endl;
    os << " wallet [options] command" << endl;
    os << endl;
    os << "options are:" << endl;
	os << " -home <homedir>   homedir. [" << p.homedir << "]" << endl;
	os << " -local    Load data from local homedir instead of connecting to a wallet daemon. [" << boolalpha << p.offline << "]" << endl;
    if (p.offline) {
	    os << " backend connector:" << endl;
	    os << " -bhost <address>  backend host. [" << p.backend_host << "]" << endl;
	    os << " -bp <port>  backend port. [" << p.backend_port << "]" << endl;
    }
    else {
	    os << " walletd connector:" << endl;
	    os << " -whost <address>  walletd address. [" << p.walletd_host << "]" << endl;
	    os << " -wp <port>  walletd port. [" << p.walletd_port << "]" << endl;
    }
    os << endl;
    os << "commands are:" << endl;
	os << " balance [0|1]          Displays the spendable amount." << endl;
	os << " address new            Generates a new key-pair, adds the private key to the wallet and prints its asociated address." << endl;
	os << " address add <privkey>  Imports a given private key in the wallet" << endl;
	os << " dump                   Lists the keys/addresses managed by wallet" << endl;
	os << " gen_keys               Generates a key pair without adding them to the wallet." << endl;
	os << " priv_key <private key> Gives information about the given private key." << endl;
//	os << " tx base                Reports the current parent block for new transactions" << endl;
//	os << " tx make <parent-block> <src account> <prev balance> <withdraw amount> <dest account> <deposit amount> <locking program hash>" << endl;
	os << " tx make_p2pkh <dest account> <amount> <fee> <sigcode_inputs=all> <sigcode_outputs=all> [<send>]" << endl;
	os << " tx decode <tx_b58>" << endl;
	os << " tx check <tx_b58>" << endl;
	os << " tx send <tx_b58>" << endl;
	os << " tx sign <tx_b58> <sigcode_inputs> <sigcode_outputs>" << endl;
	os << "    sigcodes: "; cash::tx::dump_sigcodes(cout); cout << endl;
	os << " daemon                 Run wallet daemon on port " << p.walletd_port << endl;
}





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
/*
void tx_make(api& wapi, args_t& args, const params& p) {
	t.parent_block=args.next<blockchain::diff::hash_t>();

	auto src=args.next<cash::hash_t>();
	auto prev_balance=args.next<cash::cash_t>();
	auto withdraw_amount=args.next<cash::cash_t>();

	auto dst=args.next<cash::hash_t>();
	auto deposit_amount=args.next<cash::cash_t>();
	auto locking_program=args.next<cash::hash_t>();

	cash::tx t;
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
*/



void tx(api& wapi, args_t& args, const params& p) {
	string command=args.next<string>();
/*
	if (command=="make") {
        	tx_make(wapi,args,p);
	}
	else
*/
	if (command=="make_p2pkh") {
        wallet::tx_make_p2pkh_input i;
        i.rcpt_addr=args.next<cash::hash_t>();
        i.amount=args.next<cash::cash_t>();
        i.fee=args.next<cash::cash_t>();
        i.sigcode_inputs=args.next<cash::tx::sigcode_t>(cash::tx::sigcode_all);
        i.sigcode_outputs=args.next<cash::tx::sigcode_t>(cash::tx::sigcode_all);
        i.sendover=args.next<string>("nopes")=="send";
        wapi.tx_make_p2pkh(i,cout);
	}
	else if (command=="sign") {
	    auto b58=args.next<string>();
	    auto sigcodei=args.next<cash::tx::sigcode_t>();
	    auto sigcodeo=args.next<cash::tx::sigcode_t>();
        wapi.tx_sign(b58,sigcodei,sigcodeo,cout);
	}
	else if (command=="decode") {
	    auto b58=args.next<string>();
        wapi.tx_decode(b58,cout);
	}
	else if (command=="check") {
	    auto b58=args.next<string>();
        wapi.tx_check(b58,cout);
	}
	else if (command=="send") {
		auto b58=args.next<string>();
		wapi.tx_send(b58,cout);
	}
/*
	else if (command=="base") {
		cash::app::query_accounts_t addresses;
		addresses.add("2vVN9EUdmZ5ypMe84JrQqwExMRjn");
		addresses.add("mok8mKgni4Bjv1z6NsE3JUDG6FG");
		wallet::accounts_query_t bases=wallet::query_accounts(p.backend_host, p.backend_port, addresses);
		bases.dump(cout);
	}
*/
	else {
		help(p);
	}
}

#include "api.h"

int main(int argc, char** argv) {
	args_t args(argc,argv);
	params p;
	string command=parse_options(args,p);

	api* papi;
	if (p.offline) {
		papi=new local_api(p.homedir,p.backend_host,p.backend_port);
	}
	else {
		papi=new rpc_api(p.walletd_host,p.walletd_port);
	}
	api& wapi=*papi;

	if (command=="tx") {
    	tx(wapi,args,p);
	}
	else if (command=="priv_key") {
		auto privkey=args.next<crypto::ec::keys::priv_t>();
		wapi.priv_key(privkey,cout);
	}
	else if (command=="address") {
		command=args.next<string>();
		if (command=="new") {
			wapi.new_address(cout);
		}
		else if (command=="add") {
			crypto::ec::keys::priv_t k=args.next<crypto::ec::keys::priv_t>();
			wapi.add_address(k,cout);
		}
		else {
			help(p);
		}
	}
	else if (command=="dump") {
		wapi.dump(cout);
	} 
	else if (command=="balance") {
		wapi.balance(args.next<bool>(false),cout);
	}
	else if (command=="gen_keys") {
		wapi.gen_keys(cout);
	}
	else if (command=="daemon") {
		run_daemon(p);
	}
	else {
		help(p);
	}
	delete papi;
	return 0;
}

