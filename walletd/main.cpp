#include <us/gov/cash.h>
#include <us/gov/crypto.h>
#include <string>
#include <chrono>
#include <thread>
#include <us/gov/cash/locking_programs/p2pkh.h>
#include <us/gov/cash/tx.h>
#include <us/gov/signal_handler.h>
#include <us/wallet/wallet.h>
#include <us/wallet/daemon.h>
#include "args.h"
#include "fcgi.h"

using namespace us::wallet;

using namespace std;


void sig_handler(int s) {
    cout << "main: Caught signal " << s << endl;
    signal_handler::_this.finish();
    signal(SIGINT,SIG_DFL);
    signal(SIGTERM,SIG_DFL);
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
    bool fcgi{false};
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
	os << " -d        Run wallet daemon on port " << p.walletd_port << endl;
	os << " -local    Load data from local homedir instead of connecting to a wallet daemon. [" << boolalpha << p.offline << "]" << endl;
	os << " -json     output json instead of text" << endl;
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
	os << " -fcgi     Run as fast-cgi. [" << (p.fcgi?"yes":"no") << "]" << endl;
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
	os << "    sigcodes are: "; cash::tx::dump_sigcodes(cout); cout << endl;
    os << " pair <pubkey> <name>   authorize the device identified by its public key to operate the wallet. Give it a name." << endl;
    os << " unpair <pubkey>        revoke authorization to the specified device." << endl;
    os << " list_devices           Show currently paired devices." << endl;

}




void error_log(const char* msg) {
   using namespace std;
/*
//   using namespace boost;
   static std::ofstream error;
   if(!error.is_open())
   {
      error.open("/tmp/errlog", ios_base::out | ios_base::app);
      error.imbue(locale(error.getloc(), new posix_time::time_facet()));
   }
   error << '[' << posix_time::second_clock::local_time() << "] " << msg << endl;
*/
}

void run_fcgi(const params& p) {
   //engine::init(); 
//   e=new engine("/var/cex",wcerr);
   //e->add_sample_liquidity(wcerr);
   try {
      Fastcgipp::Manager<w3api::fcgi_t> fcgi;
      fcgi.start();
      fcgi.join();
      cerr << "us-wallet fast-cgi initiated normally" << endl;
   }
   catch(std::exception& ex) {
      cerr << ex.what() << endl;
   }
//   delete e;
}


void run_daemon(const params& p) {
	signal(SIGINT,sig_handler);
	signal(SIGTERM,sig_handler);
	signal(SIGPIPE, SIG_IGN);

	wallet_daemon d(p.walletd_port, p.homedir, p.backend_host, p.backend_port);
	d.run();
}

#include <us/wallet/protocol.h>


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
        else if (cmd=="-d") {
        	p.daemon=true;
        }
        else if (cmd=="-fcgi") {
        	p.fcgi=true;
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

#include <us/wallet/api.h>

int main(int argc, char** argv) {
	args_t args(argc,argv);
	params p;
	string command=parse_options(args,p);

    if (p.daemon && p.fcgi) {
        cerr << "-d and -fcgi options are incompatible" << endl;
        return 1;
    }

    if (p.daemon) {
        run_daemon(p);
        return 0;
    }
    if (p.fcgi) {
        run_fcgi(p);
        return 0;
    }

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
            cout << "Address: ";
			wapi.new_address(cout);
		}
		else if (command=="add") {
			crypto::ec::keys::priv_t k=args.next<crypto::ec::keys::priv_t>();
            cout << "Address: ";
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
	else if (command=="pair") {
		api::pub_t pub=args.next<api::pub_t>();
	    auto name=args.next<string>();
cout << "-- " << name << endl;
		wapi.pair(pub,name,cout);
	}
	else if (command=="unpair") {
		api::pub_t pub=args.next<api::pub_t>();
		wapi.unpair(pub,cout);
	}
	else if (command=="list_devices") {
		wapi.list_devices(cout);
	}
	else {
		help(p);
	}
	delete papi;
	return 0;
}

