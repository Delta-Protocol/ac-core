#include <gov/auth.h>
#include <gov/blockchain.h>
#include <gov/cash.h>
#include <gov/rep.h>
#include <gov/signal_handler.h>
#include <gov/socket/datagram.h>
#include <gov/blockchain/protocol.h>

using namespace usgov; using namespace std;
using socket::datagram;

void sig_handler(int s) {
    cout << "main: Caught signal " << s << endl;
    signal_handler::_this.finish();
    signal(SIGINT,SIG_DFL);
    signal(SIGTERM,SIG_DFL);
}

using namespace std::chrono_literals;

struct params {
	params(): port(16672), edges(10) {
	}
	uint16_t port;
	uint16_t edges;
	bool genesis{false};
	string genesis_address;
	bool daemon{false};
	bool shell{false};
	uint16_t simul_num_nodes_deployed{0};
    string homedir;
};

void help() {
	params p;
	cout << "us-govd [options]" << endl;
	cout << "Options are:" << endl;
	cout << "  -d            Run daemon " << endl;
	cout << "  -ds           Run daemon with sysop shell" << endl;
	cout << "  -p <port>     Listening port, default " << p.port << endl;
	cout << "  -e <edges>    Max num neightbours, default " << p.edges << endl;
	cout << "  -genesis <address of genesis node>    start a new history" << endl;
	cout << "  -home <homedir>   Set home directory (default is ~/.us) " << endl;
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
		if (inp=="-genesis") {
			p.genesis=true;
			if (i<argc) {
				p.genesis_address=argv[i++];
			}
			else {
				cerr << "I need the address of the genesis node." << endl;
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
		if (inp=="-deploy_ports ") {
			if (i<argc) {
				string se=argv[i++];
				try {
					p.simul_num_nodes_deployed=stoi(se);
				}
				catch(...) {
					cerr << "num nodes is not a valid number";
					return false;
				}
			}
			else {
				cerr << "I need the number of nodes." << endl;
				return false;
			}
			continue;
		}
		if (inp=="-h") {
			help();
			exit(0);
		}
		cerr << "Unrecognized option " << inp << ". invoke with -h for help." << endl;
		return false;
	}
	return true;
}

#include <gov/crypto.h>
#include <string.h>
#include <iomanip>
#include <gov/cfg.h>

struct cfg: filesystem::cfg {
	typedef crypto::ec::keys keys_t;

	cfg(const string& privkb58, const string& blocksdir, const string& lockingdir, vector<string>&& seed_nodes): keys(privkb58), blocksdir(blocksdir), lockingdir(lockingdir), seed_nodes(seed_nodes) {
	}

	static cfg load(const string& home) {
        if (!ensure_dir(home)) {
            cerr << "Cannot create home dir " << home << endl;
            exit(1);
        }
	    string keyfile=abs_file(home,"k");
		cout << "Loading conf from " << keyfile << endl;
		if (!file_exists(keyfile)) {
			cout << "Generating cryptographic keys..."; cout.flush();
			crypto::ec::keys k=crypto::ec::keys::generate();
			ofstream f(keyfile);
			f << k.priv.to_b58() << endl;
			cout << "done." << endl;
		}
		string pk;
		{
		ifstream f(keyfile);
		getline(f,pk);
		}

		vector<string> addrs;		
		string seeds_file=abs_file(home,"nodes.manual");
		cout << "reading " << seeds_file << endl;
		ifstream f(seeds_file);
		while(f.good()) {
			string addr;
			getline(f,addr);
			if (addr.empty()) continue;
			addrs.push_back(addr);
		}
		cout << "loaded ip address of " << addrs.size() << " seed nodes" << endl;

	    string blocks_dir=abs_file(home,"blocks");
	    if (!ensure_dir(blocks_dir)) {
		cerr << "Cannot create blocks dir " << blocks_dir << endl;
		exit(1);
	    }

	    string locking_dir=abs_file(home,"locking");
		    if (!ensure_dir(locking_dir)) {
			cerr << "Cannot create locking-programs dir " << locking_dir << endl;
			exit(1);
	    }

	    return cfg(pk,blocks_dir,locking_dir,move(addrs));
	}
	string home;
	string blocksdir;
	string lockingdir;
	keys_t keys;
	vector<string> seed_nodes;
};

struct thinfo {
	thinfo(const params& p, const cfg& conf): p(p), conf(conf) {
	}
	const params& p;
	const cfg& conf;
	blockchain::peer_t* peer{0};

};

condition_variable cv22;
mutex mx22;
bool ready22{false};
bool finished22{false};
void shell_echo(thinfo* info) {
	blockchain::peer_t&cli=*info->peer;
	fd_set read_fd_set;
	blockchain::daemon demon(info->conf.keys);
	demon.sysop_allowed=true;
	cli.parent=&demon.peerd;

	while (!finished22) {
		FD_ZERO(&read_fd_set);
		FD_SET(cli.sock,&read_fd_set);
		if (::select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0) {
			cout << "Error in select" << endl;
			continue;
		}
		if (FD_ISSET (cli.sock, &read_fd_set)) {
			datagram* d=cli.complete_datagram();
			if (!d || d->error!=0) {
				cout << "error recv datagram." << endl;
				if (d) delete d;
				return;
			}
			if (!d->completed()) { 
				return;
			}
			if (demon.peerd.process_work_sysop(&cli,d)) {
				continue;
			}
			if (d->service!=usgov::protocol::sysop) {
				delete d;
				continue;
			}
			cout << d->parse_string() << endl;
			delete d;
			{
			unique_lock<mutex> lock(mx22);
			ready22=true;
			}
			cv22.notify_all();
		}
	}
}

#include <gov/blockchain/protocol.h>

void open_shell(thinfo& i) {
	blockchain::peer_t cli(0);
	i.peer=&cli;
	
	cout << "us.gov Introspective Shell" << endl;
	cout << "Connecting to daemon at localhost:" << i.p.port << endl;
	if (!cli.connect("127.0.0.1",i.p.port,true)) {
		cerr << "Cannot connect" << endl;
		return;
	}
	cout << "Start thread" << endl;
	thread th(&shell_echo,&i);
	cout << "/Start thread" << endl;
	cout << "Connecting to daemon at localhost:" << i.p.port << endl;

	//cli.ping();

	cout << "Authenticating..."; cout.flush();
	while (true) {
		if (cli.stage_peer==auth::peer_t::verified && cli.stage_me==auth::peer_t::verified) break;
		if (cli.stage_peer==auth::peer_t::verified_fail || cli.stage_me==auth::peer_t::verified_fail) {
			cerr << "Authentication failed." << endl;
			exit(1);
		}
		if (cli.stage!=blockchain::peer_t::unknown && cli.stage!=blockchain::peer_t::sysop) {
			cerr << "Authentication failed." << endl;
			exit(1);
		}

		this_thread::sleep_for(chrono::seconds(1));
		cout << "."; cout.flush();		
	}	
	cout << endl;
	while (true) {
		cout << "> "; cout.flush();
		string line;
		getline(cin,line);
		if (line=="q") {
			cout << "quitting.." << endl;
			break;
		}
		datagram* d=new datagram(usgov::protocol::sysop,line);
//cout << "sending " << usgov::protocol::sysop << " " <<line << endl;
		cli.send(d);
		{
		unique_lock<mutex> lock(mx22);
		ready22=false;
		cv22.wait(lock,[]{return ready22;});
		}

	}
	finished22=true;
	cli.send(new datagram(protocol::ping));
	this_thread::sleep_for(chrono::seconds(1));
	cli.disconnect();
	th.join();
	
}

#include <cstdlib>
#include <gov/crypto.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
	params p;
	if(!parse_cmdline(argc,argv,p)) return 1;


	string home;
    if (p.homedir.empty()) {
        const char* env_p = std::getenv("HOME");
        if (!env_p) {
            cerr << "No $HOME env var defined" << endl;
            exit(1);
        }
        home=env_p;
    	home+="/.us";
    }
    else {
        home=p.homedir;
    }

    cout << "home dir: " << home << endl;

	if (!cfg::ensure_dir(home)) {
		cerr << "Cannot create dir " << home << endl;
		exit(1);
	}
	home+="/gov";

	if (p.genesis) {
		if (cfg::exists_dir(home)) {
			cerr << "Cannot start a new blockchain if home dir exists: " << home << endl;
			exit(1);
		}
	}

	cfg conf=cfg::load(home);
	if (p.daemon) {
		cout << "Node public key is " << conf.keys.pub << endl;
		signal(SIGINT,sig_handler);
		signal(SIGTERM,sig_handler);
		signal(SIGPIPE, SIG_IGN);
		blockchain::daemon d(conf.keys,conf.blocksdir,p.port,p.edges,conf.seed_nodes);
		d.sysop_allowed=p.shell;
		d.add(new cash::app());
		d.add(new rep::app());

        if (p.genesis) {
            cout << "Adding genesis block with 1 node " << p.genesis_address << endl;
//            d.auth_app->db.genesis(conf.keys.pub,p.genesis_address);
            d.start_new_blockchain(p.genesis_address);
            cout << "genesis block has been created." << endl;
            exit(0);
        }


		d.run();
		cout << "main: exited normally" << (thread_::_this.terminated?", terminated by signal":"") << endl;
	}
	else {
		thinfo i(p,conf);
		open_shell(i);
	}
	return 0;
}

