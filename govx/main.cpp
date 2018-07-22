#include <us/gov/auth.h>
#include <us/gov/blockchain.h>
#include <us/gov/cash.h>
#include <us/gov/signal_handler.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/blockchain/protocol.h>
#include <us/gov/input.h>
#include <us/gov/blockchain/protocol.h>
#include <us/gov/auth/peer_t.h>

using namespace us::gov;
using namespace std;
using namespace std::chrono_literals;
using socket::datagram;
using us::gov::input::cfg_id;

void sig_handler(int s) {
    cout << "main: Caught signal " << s << endl;
    signal_handler::_this.finish();
    signal(SIGINT,SIG_DFL);
    signal(SIGTERM,SIG_DFL);
    signal(SIGPIPE,SIG_DFL);
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
        if (daemon) {
            os << "run daemon" << endl;
            os << "  listening port: " << port << endl;
            os << "  edges: " << edges << endl;
            os << "  sysop shell: " << boolalpha << shell << endl;
        }
        else {
            os << "run rpc shell" << endl;
            os << "  host: " << sysophost << ":" << port << endl;
        }
    }

	uint16_t port{16672};
	uint16_t edges{10};
	bool daemon{false};
	bool shell{false};
    string homedir;
    string sysophost{"127.0.0.1"};
};

void help(const params& p, ostream& os) {
	os << "us-govd [options]" << endl;
	os << "Options are:" << endl;
	os << "  -d            Run daemon " << endl;
	os << "  -ds           Run daemon with sysop shell. " << boolalpha << p.shell << endl;
	os << "  -p <port>     Listening port. " << p.port << endl;
	os << "  -e <edges>    Max num neightbours " << p.edges << endl;
	os << "  -home <homedir>   Set home directory. " << p.homedir << endl;
	os << "  -host <address>   daemon host. " << p.sysophost << endl;
	os << "  -h            Print this help and exit " << endl;
}


using us::gov::input::args_t;

string parse_options(args_t& args, params& p) {
    string cmd;
    while(true) {
        cmd=args.next<string>();
		if (cmd=="-p") {
            p.port=args.next<uint16_t>();
		}
		else if (cmd=="-d") {
			p.daemon=true;
		}
		else if (cmd=="-ds") {
			p.daemon=true;
			p.shell=true;
		}
		else if (cmd=="-e") {
            p.edges=args.next<uint16_t>();
		}
		else if (cmd=="-home") {
			p.homedir=args.next<string>();
		}
		else if (cmd=="-host") {
			p.sysophost=args.next<string>();
		}
		else if (cmd=="-h") {
			help(p,cout);
			exit(0);
		}
        else {
            break;
        }
    }
    return cmd;
}

struct shell_client: us::gov::auth::peer_t {
	shell_client(const keys& k): k(k) {
	}
        virtual const keys& get_keys() const override {
		return k;
	}
    virtual bool authorize(const pubkey_t& p) const override {
        return true;
    }

	const keys& k;
};

void open_shell(const params&p) {
	using us::gov::input::cfg_id;

	string govhomedir=p.homedir+"/gov";
	string homedir=govhomedir+"/rpc_client";
	cfg_id conf=cfg_id::load(homedir);


	shell_client peer(conf.keys);
    auto r=peer.connect(p.sysophost,p.port,true);
    if (!r.empty()) {
        p.dump(cerr);
		cerr << "Cannot connect to " << p.sysophost << ":" << p.port << ". " << r << endl;
		return;
	}
	r=peer.run_auth_responder();
	if (!r.empty()) {
        p.dump(cerr);
		cerr << r << endl;
		return;
	}

    {
        auto r=peer.recv();
        if (!r.first.empty()) {
            assert(r.second==0);
            p.dump(cerr);
            cerr << r.first << endl;
            cerr << "us.gov is rejecting sysop connections. See -ds option." << endl;
            return;
        }
        assert(r.second!=0);
        auto s=r.second->parse_string();
        delete r.second;
        if (s!="go ahead") {
            p.dump(cerr);
            cerr << "unknown protocol" << endl;
            return;
        }
    }

	cout << "*us.gov* Introspective Shell" << endl;
	cout << "Connected to us.gov at " << p.sysophost << ":" << p.port << endl;
	cout << "Type h for help." << endl;

	while (!signal_handler::_this.terminated) {
		cout << "> "; cout.flush();
		string line;
		getline(cin,line);
		if (line=="q") {
			break;
		}
		datagram* d=new datagram(us::gov::protocol::sysop,line);
		auto ans=peer.send_recv(d);
		if (!ans.first.empty()) {
			cerr << ans.first << endl;
			break;
		}
		cout << ans.second->parse_string() << endl;
		delete ans.second;
	}
}

void run_daemon(const params&p) {
    p.dump(cout);
	using us::gov::input::cfg_daemon;
	string homedir=p.homedir+"/gov";
	cfg_daemon conf=cfg_daemon::load(homedir);
	cout << "Node public key is " << conf.keys.pub << " address " << conf.keys.pub.hash() << endl;
	blockchain::daemon d(conf.keys,conf.home,p.port,p.edges,conf.seed_nodes);
	d.sysop_allowed=p.shell;
	d.add(new cash::app());
	d.run();
	cout << "main: exited normally" << (thread_::_this.terminated?", terminated by signal":"") << endl;
}

//daemon only allows sysop connection from the same owner (i.e. same priv key k)
//if a gov daemon is running in the same homedir
//copy daemon priv key into the rpc_client device-id
void sysop_localhost_allow(const params& p) {
	string homedir=p.homedir+"/gov";
	string rpchomedir=homedir+"/rpc_client";
    auto k=cfg_id::load_priv_key(homedir);
    auto rpck=cfg_id::load_priv_key(rpchomedir);
    if (k.first && !rpck.first) {
       if (!p.daemon) {
            cfg_id::write_k(rpchomedir,k.second);
       }
    }
    else if (!k.first && rpck.first) {
       if (p.daemon) {
            cfg_id::write_k(homedir,rpck.second);
       }
    }
}


/// \brief  Main function
/// \param  argc An integer argument count of the command line arguments
/// \param  argv An argument vector of the command line arguments
/// \return an integer 0 upon exit success
int main(int argc, char** argv) {
    args_t args(argc,argv);
    params p;
    string command=parse_options(args,p);

    if (!command.empty()) {
        cerr << "Error. Unrecognized command " << command << endl;
        exit(1);
    }

    sysop_localhost_allow(p);

	signal(SIGINT,sig_handler);
	signal(SIGTERM,sig_handler);
	signal(SIGPIPE, SIG_IGN);

	if (p.daemon) {
		run_daemon(p);
	}
	else { //RPC to govd - sysop
		open_shell(p);
	}
	return 0;
}





/** @file */    
//Doxygen stupidity, thanks anyway
