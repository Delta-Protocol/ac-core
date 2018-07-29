#include <us/gov/cash.h>
#include <us/gov/crypto.h>
#include <string>
#include <chrono>
#include <thread>
#include <us/gov/cash/locking_programs/p2pkh.h>
#include <us/gov/cash/tx.h>
#include <us/gov/input/cfg.h>
#include <us/gov/signal_handler.h>
#include <us/wallet/wallet.h>
#include <us/wallet/daemon.h>
#include <us/gov/input.h>
#include <us/wallet/daemon_api.h>
#include "fcgi.h"
#include <us/wallet/json/daemon_api.h>

using namespace us::wallet;

using namespace std;

#ifdef FCGI
Fastcgipp::Manager<w3api::fcgi_t>* fcgi{0};
#endif

void sig_handler(int s) {
    cout << "main: Caught signal " << s << endl;
    signal_handler::_this.finish();
#ifdef FCGI
    if (fcgi) fcgi->terminate();
#endif
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
        os << "gov: " << backend_host << ":" << backend_port << endl;
        os << "wallet: " << walletd_host << ":" << walletd_port << endl;
    }
	bool daemon{false};
#ifdef FCGI
    bool fcgi{false};
#endif
    bool json{false};
    bool help{false};
    bool advanced{false};
    uint16_t listening_port{16673};
    string homedir;
    bool offline{false};
    string backend_host{"localhost"}; uint16_t backend_port{16672};
    string walletd_host{"localhost"}; uint16_t walletd_port{16673};
};


void help(const params& p, ostream& os=cout) {
    os << "us-wallet (";
    if (p.offline)
        os << "local";
    else 
        os << "rpc";
    os << " mode)" << endl;
    os << "You can use this software under the terms of the Affero General Public License (AGPL)." << endl;
    os << "Obtain a copy of the licence here: https://www.gnu.org/licenses/agpl.txt" << endl;
    os << endl;
    os << "Usage: us-wallet [options] [command]" << endl;
    os << "options are:" << endl;
    if (!p.advanced) {
	  os << " -a                Advanced mode." << endl;
    }
    if (p.advanced) {
      if (!p.offline) {
	    os << " -local    Local mode. Use a wallet directory, instead of connecting to a wallet daemon. [" << boolalpha << p.offline << "]" << endl;
      }
      os << " -home <homedir>   homedir. [" << p.homedir << "]  requires -local" << endl;
	  os << " -d        Run a new wallet-daemon listening on port " << p.listening_port << " (see -lp). Ignored with -local" << endl;
	  os << " -lp       Listening Port. Ignored with -local" << endl;
#ifdef FCGI
	  os << " -fcgi     Behave as a fast-cgi program. Requires -d. [" << (p.fcgi?"yes":"no") << "]" << endl;
#endif
	  os << " -json     output json instead of text. [" << boolalpha << p.json << "]" << endl;
      os << " RPC to backend(us-gov) parameters:  Requires -local" << endl;
      os << "   -bhost <address>  us-gov IP address. [" << p.backend_host << "]" << endl;
      os << "   -bp <port>  TCP port. [" << p.backend_port << "]" << endl;
      os << " RPC to wallet-daemon (us-wallet) parameters: Conflicts with -local" << endl;
	  os << "   -whost <address>  walletd address. [" << p.walletd_host << "]" << endl;
	  os << "   -wp <port>  walletd port. [" << p.walletd_port << "]" << endl;
    }
    os << endl;
    os << "commands are:" << endl;
    os << "cash:" << endl;

	os << "  balance [detailed=0|1]          Displays the spendable amount. Default value for detailed is 0." << endl;
	os << "  transfer <dest account> <receive amount>      Order a cash transfer to <dest account>. Fees are paid by you, the sender." << endl;
    if (p.advanced) {
	  os << "  tx make_p2pkh <dest account> <amount> <fee> <sigcode_inputs=all> <sigcode_outputs=all> [<send>]" << endl;
	  os << "    sigcodes are: "; cash::tx::dump_sigcodes(cout); cout << endl;
	  os << "  tx decode <tx_b58>" << endl;
	  os << "  tx check <tx_b58>" << endl;
	  os << "  tx send <tx_b58>" << endl;
	  //os << "  tx sign <tx_b58> <sigcode_inputs> <sigcode_outputs>" << endl;  //hidden fuctionality
    }
    os << endl;
    os << "keys: (more in advanced mode)" << endl;
	os << "  address new            Generates a new key-pair, adds the private key to the wallet and prints its asociated address." << endl;
    if (p.advanced) {
	  os << "  address add <privkey>  Imports a given private key in the wallet" << endl;
    }
	os << "  list [show_priv_keys=0|1]   Lists all keys. Default value for show_priv_keys is 0." << endl;
    if (p.advanced) {
	  os << "  gen_keys               Generates a key pair without adding them to the wallet." << endl;
	  os << "  priv_key <private key> Gives information about the given private key." << endl;
    }
    os << endl;
    if (p.advanced) {
      os << "device pairing:" << endl;
      os << "  device_id              Show this device Id. (Only RPC mode)" << endl;
      os << "  pair <pubkey> <name>   Authorize the device identified by its public key to operate the wallet. Give it a name." << endl;
      os << "  unpair <pubkey>        Revoke authorization to the specified device." << endl;
      os << "  list_devices           Prints the list of recognized devices." << endl;
      os << endl;

      os << "net:" << endl;
      os << "  ping                   endpoint check" << endl;
      os << endl;
    }
}

#ifdef FCGI
void run_fcgi(const params& p) {
   try {
      fcgi=new Fastcgipp::Manager<w3api::fcgi_t>();
      //fcgi.setupSignals();
      fcgi->listen();
      fcgi->start();
      cout << "us-wallet fast-cgi initiated normally" << endl;
      fcgi->join();
      delete fcgi;
      fcgi=0;
   }
   catch(std::exception& ex) {
      cerr << ex.what() << endl;
   }
}
#endif

#include <us/wallet/protocol.h>

using us::gov::input::shell_args;

string parse_options(shell_args& args, params& p) {
    string cmd;
    while(true) {
    	cmd=args.next<string>();
        if (cmd=="-wp") {
        	p.walletd_port=args.next<int>();
        }
        else if (cmd=="-lp") {
        	p.listening_port=args.next<uint16_t>();
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
        else if (cmd=="-a") {
        	p.advanced=true;
        }
#ifdef FCGI
        else if (cmd=="-fcgi") {
        	p.fcgi=true;
        }
#endif
        else if (cmd=="-json") {
        	p.json=true;
        }
        else if (cmd=="-h") {
        	p.help=true;
        }
        else {
            break;
        }
    }
    return cmd;
}

void tx(us::wallet::wallet_api& wapi, shell_args& args, const params& p, ostream& os) {
	string command=args.next<string>();
	if (command=="transfer") {
        wapi.transfer(args.next<cash::hash_t>(),args.next<cash::cash_t>(),os);
	}
	else if (command=="make_p2pkh") {
        wallet::tx_make_p2pkh_input i;
        i.rcpt_addr=args.next<cash::hash_t>();
        i.amount=args.next<cash::cash_t>();
        i.fee=args.next<cash::cash_t>();
        i.sigcode_inputs=args.next<cash::tx::sigcode_t>(cash::tx::sigcode_all);
        i.sigcode_outputs=args.next<cash::tx::sigcode_t>(cash::tx::sigcode_all);
        i.sendover=args.next<string>("nopes")=="send";
        wapi.tx_make_p2pkh(i,os);
	}
	else if (command=="sign") {
	    auto b58=args.next<string>();
	    auto sigcodei=args.next<cash::tx::sigcode_t>();
	    auto sigcodeo=args.next<cash::tx::sigcode_t>();
        wapi.tx_sign(b58,sigcodei,sigcodeo,os);
	}
	else if (command=="decode") {
	    auto b58=args.next<string>();
        wapi.tx_decode(b58,os);
	}
	else if (command=="check") {
	    auto b58=args.next<string>();
        wapi.tx_check(b58,os);
	}
	else if (command=="send") {
		auto b58=args.next<string>();
		wapi.tx_send(b58,os);
	}
	else {
		help(p);
	}
}

#include <us/wallet/rpc_api.h>
#include <us/wallet/wallet_local_api.h>
#include <us/wallet/pairing_local_api.h>

using us::gov::input::cfg;
using us::gov::input::cfg0;
using us::gov::input::cfg1;

void run_daemon(const params& p) {
	signal(SIGINT,sig_handler);
	signal(SIGTERM,sig_handler);
	signal(SIGPIPE, SIG_IGN);

//	us::gov::filesystem::cfg cfg=us::gov::filesystem::cfg::load(p.homedir);
    string homedir=p.homedir+"/wallet";
	wallet_daemon* d=new wallet_daemon(cfg1::load(homedir).keys, p.listening_port, homedir, p.backend_host, p.backend_port);

    auto k=cfg1::load_priv_key(p.homedir+"/gov");
    if (k.first) d->add_address(k.second);

#ifdef FCGI
    if (p.fcgi) {
        api* a=d;
        if (p.json) {
            a=new json_api(a);
        }
        thread* wt=0;
        w3api::fcgi_t::api=a;

        wt=new thread([&](){d->run(); if (fcgi) fcgi->terminate(); });
        run_fcgi(p);
        wt->join();
        delete wt;
        w3api::fcgi_t::api=0;
    }
    else {
        d->run();
    }
#else
    d->run();
#endif
    delete d;
}

using us::gov::input::cfg_id;

//if a gov daemon is running in the same homedir
//import gov priv key into the wallet
void import_gov_k(us::wallet::wallet_api& x, const params& p) {
    string govhomedir=p.homedir+"/gov";
    if (!cfg_id::file_exists(cfg_id::k_file(govhomedir))) {
        return;
    }
    auto k=cfg_id::load_priv_key(govhomedir);
    if (k.first) {
        ostream nullos(0);
        x.add_address(k.second,nullos);
    }
    else {
        cerr << "Error. cannot read key file " << cfg_id::k_file(govhomedir) << endl;
        exit(1);
    }
}

#include <us/wallet/daemon_local_api.h>
#include <us/wallet/daemon_rpc_api.h>

void run_local(string command, shell_args& args, const params& p) {
	auto homedir=p.homedir+"/wallet";
	auto gov_homedir=p.homedir+"/wallet";

	us::wallet::daemon_api* papi;
	if (p.offline) {
		cfg0::load(homedir);
		papi=new us::wallet::daemon_local_api(homedir,p.backend_host,p.backend_port); //rpc to node
        import_gov_k(*papi,p);
	}
	else {
		using us::gov::input::cfg_id;
		auto f=cfg_id::load(homedir+"/rpc_client");
		papi=new us::wallet::daemon_rpc_api(f.keys, p.walletd_host,p.walletd_port); //rpc to a wallet daemon, same role as android app
	}

    if (p.json) papi=new us::wallet::json::daemon_api(papi);

	us::wallet::daemon_api& wapi=*papi;
    typedef us::wallet::daemon_api::pub_t pub_t;

    ostringstream os;

	if (command=="transfer") { //shortcut to tx fn
		command="tx";
		--args.n; //repeat command transfer
	}

	if (command=="tx") {
    	tx(wapi,args,p,os);
	}
	else if (command=="priv_key") {
		auto privkey=args.next<crypto::ec::keys::priv_t>();
		wapi.priv_key(privkey,os);
	}
	else if (command=="address") {
		command=args.next<string>();
		if (command=="new") {
			wapi.new_address(os);
		}
		else if (command=="add") {
			crypto::ec::keys::priv_t k=args.next<crypto::ec::keys::priv_t>();
			wapi.add_address(k,os);
		}
		else {
			help(p);
		}
	}
	else if (command=="list") {
		wapi.list(args.next<bool>(false),os);
	} 
	else if (command=="balance") {
		wapi.balance(args.next<bool>(false),os);
	}
	else if (command=="gen_keys") {
		wapi.gen_keys(os);
	}
    else if (command=="device_id") {
        if (p.offline) {
            cout << "Not such a thing for a local wallet";
        }
        else {
            auto f=cfg_id::load(homedir+"/rpc_client");
            cout << f.keys.pub;
        }
    }
	else if (command=="pair") {
		pub_t pub=args.next<pub_t>();
        if (!pub.valid) {
            os << "Error: Invalid public key";
        }
        else {
    	    auto name=args.next<string>();
	    	wapi.pair(pub,name,os);
        }
	}
	else if (command=="unpair") {
		pub_t pub=args.next<pub_t>();
		wapi.unpair(pub,os);
	}
	else if (command=="list_devices") {
		wapi.list_devices(os);
	}
	else if (command=="ping") {
   		wapi.ping(os);
	}
	else {
        cerr << "Invalid command " << command << endl;
		help(p);
	}
	delete papi;
    cout << os.str() << endl;
}

/// \brief  Main function
/// \param  argc An integer argument count of the command line arguments
/// \param  argv An argument vector of the command line arguments
/// \return an integer 0 upon exit success
int main(int argc, char** argv) {
	shell_args args(argc,argv);
	params p;
	string command=parse_options(args,p);

    if (p.help) {
        help(p);
        return 0;
    }

#ifdef FCGI
    if (!p.daemon && p.fcgi) {
        cerr << "-fcgi requires -d" << endl;
        return 1;
    }
#endif

    if (p.daemon) {
        if (p.offline) {
            cerr << "-local and -d cannot be used together." << endl;
            return 1;
        }
        run_daemon(p);
        return 0;
    }
    run_local(command,args,p);
    return 0;
}










/** @file */
//Doxygen stupidity, thanks anyway

