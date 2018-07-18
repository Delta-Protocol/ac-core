#include <us/gov/auth.h>
#include <us/gov/blockchain.h>
#include <us/gov/cash.h>
#include <us/gov/signal_handler.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/blockchain/protocol.h>
#include <us/gov/input.h>

using namespace us::gov;
using namespace std;
using socket::datagram;

void sig_handler(int s) {
    cout << "main: Caught signal " << s << endl;
    signal_handler::_this.finish();
    signal(SIGINT,SIG_DFL);
    signal(SIGTERM,SIG_DFL);
    signal(SIGPIPE,SIG_DFL);
}

using namespace std::chrono_literals;

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

void help(const params& p) {
	cout << "us-govd [options]" << endl;
	cout << "Options are:" << endl;
	cout << "  -d            Run daemon " << endl;
	cout << "  -ds           Run daemon with sysop shell. " << boolalpha << p.shell << endl;
	cout << "  -p <port>     Listening port. " << p.port << endl;
	cout << "  -e <edges>    Max num neightbours " << p.edges << endl;
	cout << "  -home <homedir>   Set home directory. " << p.homedir << endl;
	cout << "  -host <address>   daemon host. " << p.sysophost << endl;
	cout << "  -h            Print this help and exit " << endl;
}

bool parse_cmdline(int argc, char** argv, params& p) {
	int i=1;
	while (i<argc) {
		string inp=argv[i++];
		if (inp=="-p") {
			if (i<argc) {
				string sp=argv[i++];
				try {
					p.port=stoi(sp);
				}
				catch(...) {
					cerr << "port is not a valid number";
					return false;
				}
			}
			else {
				cerr << "I need the port number, please." << endl;
				return false;
			}
			continue;
		}
		if (inp=="-d") {
			p.daemon=true;
			continue;
		}
		if (inp=="-ds") {
			p.daemon=true;
			p.shell=true;
			continue;
		}
		if (inp=="-e") {
			if (i<argc) {
				string se=argv[i++];
				try {
					p.edges=stoi(se);
				}
				catch(...) {
					cerr << "num edges is not a valid number";
					return false;
				}
			}
			else {
				cerr << "I need the number of edges." << endl;
				return false;
			}
			continue;
		}
		if (inp=="-home") {
			if (i<argc) {
				p.homedir=argv[i++];
			}
			else {
				cerr << "I need a directory." << endl;
				return false;
			}
			continue;
		}
		if (inp=="-host") {
			if (i<argc) {
				p.sysophost=argv[i++];
			}
			else {
				cerr << "I need an address." << endl;
				return false;
			}
			continue;
		}
		if (inp=="-h") {
			help(p);
			exit(0);
		}
		cerr << "Unrecognized option " << inp << ". invoke with -h for help." << endl;
		return false;
	}
	return true;
}

#include <us/gov/crypto.h>
#include <string.h>
#include <iomanip>
#include <fstream>
#include <us/gov/input.h>

using us::gov::input::cfg_id;

#include <us/gov/blockchain/protocol.h>
#include <us/gov/auth/peer_t.h>

struct shell_client: us::gov::auth::peer_t {
	shell_client(const keys& k): k(k) {
	}
        virtual const keys& get_keys() const override {
		return k;
	}
	const keys& k;
};

void open_shell(const params&p) {
	using us::gov::input::cfg_id;
	string homedir=p.homedir+"/gov";
	cfg_id conf=cfg_id::load(homedir);

	shell_client peer(conf.keys);
	if (!peer.connect(p.sysophost,p.port,true)) {
		cerr << "Cannot connect to " << p.sysophost << ":" << p.port << endl;
		return;
	}
	auto r=peer.run_auth();
	if (!r.empty()) {
		cerr << r << endl;
		return;
	}

    {
        auto r=peer.recv();
        if (!r.first.empty()) {
            cerr << r.first << endl;
            cerr << "us.gov is rejecting sysop connections. See -ds option." << endl;
            return;
        }
        if (r.second->parse_string()!="go ahead") {
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
	using us::gov::input::cfg_daemon;
	string homedir=p.homedir+"/gov";
	cfg_daemon conf=cfg_daemon::load(homedir);
	cout << "Node public key is " << conf.keys.pub << endl;
	blockchain::daemon d(conf.keys,conf.home,p.port,p.edges,conf.seed_nodes);
	d.sysop_allowed=p.shell;
	d.add(new cash::app());
	d.run();
	cout << "main: exited normally" << (thread_::_this.terminated?", terminated by signal":"") << endl;
}

int main(int argc, char** argv) {
	params p;
	if(!parse_cmdline(argc,argv,p)) return 1;
    p.dump(cout);

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

