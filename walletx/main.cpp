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
#include "fcgi.h"

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
    bool json{false};
#endif
    bool advanced{false};
    uint16_t listening_port{16673};
    string homedir;
    bool offline{false};
    string backend_host{"localhost"}; uint16_t backend_port{16672};
    string walletd_host{"localhost"}; uint16_t walletd_port{16673};
};


void help(const params& p, ostream& os=cout) {
    os << "us-wallet" << endl;
    os << "You can use this software under the terms of the Affero General Public License (AGPL)." << endl;
    os << "Obtain a copy of the licence here: https://www.gnu.org/licenses/agpl.txt" << endl;
    os << endl;
    os << "Usage: us-wallet [options] [command]" << endl;
    os << "options are:" << endl;
    if (!p.advanced) {
	  os << " -a                Advanced mode." << endl;
    }
    if (p.advanced) {
      if (p.offline) {
	    os << " -home <homedir>   homedir. [" << p.homedir << "]" << endl;
      }
	  os << " -d        Run a new wallet-daemon listening on port " << p.listening_port << " (see -lp)" << endl;
	  os << " -lp       Listening Port." << endl;
	  os << " -local    Load data from local homedir instead of connecting to a wallet daemon. [" << boolalpha << p.offline << "]" << endl;
#ifdef FCGI
	  os << " -fcgi     Behave as a fast-cgi program. Requires -d. [" << (p.fcgi?"yes":"no") << "]" << endl;
	  os << " -json     output json instead of text. [" << boolalpha << p.json << "]" << endl;
#endif
      if (p.offline) {
	     os << " RPC to backend(us-gov) parameters:" << endl;
	     os << " -bhost <address>  us-gov IP address. [" << p.backend_host << "]" << endl;
	     os << " -bp <port>  TCP port. [" << p.backend_port << "]" << endl;
      }
      else {
	     os << " RPC to wallet-daemon (us-wallet) parameters:" << endl;
	     os << " -whost <address>  walletd address. [" << p.walletd_host << "]" << endl;
	     os << " -wp <port>  walletd port. [" << p.walletd_port << "]" << endl;
      }
    }
    os << endl;
    os << "commands are:" << endl;
    os << "cash:" << endl;

	os << "  balance [0|1]          Displays the spendable amount." << endl;
	os << "  transfer <dest account> <receive amount>      Orders a transfer to <dest account> for an amount. Fees are paid by the sender." << endl;
    if (p.advanced) {
	  os << "  tx make_p2pkh <dest account> <amount> <fee> <sigcode_inputs=all> <sigcode_outputs=all> [<send>]" << endl;
	  os << "  tx decode <tx_b58>" << endl;
	  os << "  tx check <tx_b58>" << endl;
	  os << "  tx send <tx_b58>" << endl;
	  os << "  tx sign <tx_b58> <sigcode_inputs> <sigcode_outputs>" << endl;
	  os << "    sigcodes are: "; cash::tx::dump_sigcodes(cout); cout << endl;
    }
    os << endl;
    os << "keys:" << endl;
	os << "  address new            Generates a new key-pair, adds the private key to the wallet and prints its asociated address." << endl;
    if (p.advanced) {
	  os << "  address add <privkey>  Imports a given private key in the wallet" << endl;
    }
	os << "  list                   Lists the keys/addresses managed by wallet" << endl;
    if (p.advanced) {
	  os << "  gen_keys               Generates a key pair without adding them to the wallet." << endl;
	  os << "  priv_key <private key> Gives information about the given private key." << endl;
    }
    os << endl;
    if (p.advanced) {
      os << "device pairing:" << endl;
      os << "  pair <pubkey> <name>   Authorize the device identified by its public key to operate the wallet. Give it a name." << endl;
      os << "  unpair <pubkey>        Revoke authorization to the specified device." << endl;
      os << "  list_devices           Prints the list of recognized devices." << endl;
      os << endl;

      os << "net:" << endl;
      os << "  ping [gov|wallet]      Daemon-is-alive check" << endl;
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

using us::gov::input::args_t;

string parse_options(args_t& args, params& p) {
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
        else if (cmd=="-json") {
        	p.json=true;
        }
#endif
        else {
            break;
        }
    }
    return cmd;
}

void tx(api& wapi, args_t& args, const params& p, ostream& os) {
	string command=args.next<string>();
	if (command=="transfer") {
        wallet::tx_make_p2pkh_input i;
        i.rcpt_addr=args.next<cash::hash_t>();
        i.amount=args.next<cash::cash_t>();
        i.fee=1;
        i.sigcode_inputs=cash::tx::sigcode_all;
        i.sigcode_outputs=cash::tx::sigcode_all;
        i.sendover=true;
        wapi.tx_make_p2pkh(i,os);
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
#include <us/wallet/local_api.h>

#ifdef FCGI
#include <jsoncpp/json/json.h> 
Json::Value to_json(const string& s) {
    istringstream is(s);
    Json::Value val;
    int i=0;
    while(is.good()) {
        string f;
        is >> f;
        if (!f.empty()) val[i++]=f;
    }
    return val;
}
#endif


using us::gov::input::cfg;
using us::gov::input::cfg1;

void run_daemon(const params& p) {
	signal(SIGINT,sig_handler);
	signal(SIGTERM,sig_handler);
	signal(SIGPIPE, SIG_IGN);

//	us::gov::filesystem::cfg cfg=us::gov::filesystem::cfg::load(p.homedir);
    string homedir=p.homedir+"/wallet";

	wallet_daemon d(cfg1::load(homedir).keys, p.listening_port, homedir, p.backend_host, p.backend_port);

    auto k=cfg1::load_priv_key(p.homedir+"/gov");
    if (k.first) d.add_address(k.second);

#ifdef FCGI
    if (p.fcgi) {
        thread* wt=0;
        w3api::fcgi_t::api=&d;

        wt=new thread([&](){d.run(); if (fcgi) fcgi->terminate(); });
        run_fcgi(p);
        wt->join();
        delete wt;
    }
    else {
        d.run();
    }
#else
    d.run();
#endif
}


void run_local(string command, args_t& args, const params& p) {
	auto homedir=p.homedir+"/wallet";
	auto gov_homedir=p.homedir+"/wallet";

	api* papi;
	if (p.offline) {
		papi=new local_api(homedir,p.backend_host,p.backend_port); //rpc to node
	}
	else {
		using us::gov::input::cfg_id;
		auto f=cfg_id::load(homedir+"/rpc_client");
		papi=new rpc_api(f.keys, p.walletd_host,p.walletd_port); //rpc to a wallet daemon, same role as android app
	}
	api& wapi=*papi;

    ostringstream os;

	if (command=="transfer") { //shortcut to tx fn
		command=="tx";
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
            cout << "Address: ";
			wapi.new_address(os);
		}
		else if (command=="add") {
			crypto::ec::keys::priv_t k=args.next<crypto::ec::keys::priv_t>();
            cout << "Address: ";
			wapi.add_address(k,os);
		}
		else {
			help(p);
		}
	}
	else if (command=="dump") {
		wapi.dump(os);
	} 
	else if (command=="balance") {
		wapi.balance(args.next<bool>(false),os);
	}
	else if (command=="gen_keys") {
		wapi.gen_keys(os);
	}
	else if (command=="pair") {
		api::pub_t pub=args.next<api::pub_t>();
        if (!pub.valid) {
            os << "Error: Invalid public key";
        }
        else {
    	    auto name=args.next<string>();
//cout << "-- " << name << endl;
	    	wapi.pair(pub,name,os);
        }
	}
	else if (command=="unpair") {
		api::pub_t pub=args.next<api::pub_t>();
		wapi.unpair(pub,os);
	}
	else if (command=="list_devices") {
		wapi.list_devices(os);
	}
	else if (command=="ping") {
		auto w=args.next<string>();
        if (w=="gov")
    		wapi.ping_gov(os);
        else
    		wapi.ping_wallet(os);
	}
	else {
		help(p);
	}
//cout << "deleting" << endl;
	delete papi;

#ifdef FCGI
    if (p.json)
    cout << to_json(os.str()) << endl;
    else
    cout << os.str() << endl;
#else
    cout << os.str() << endl;
#endif
}



int main(int argc, char** argv) {
	args_t args(argc,argv);
	params p;
	string command=parse_options(args,p);

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

