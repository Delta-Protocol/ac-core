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
        os << "sysophost: " << sysophost << ":" << port << endl;
        os << "edges: " << edges << endl;
        os << "port: " << port << endl;
    }

	uint16_t port{16672};
	uint16_t edges{10};
	bool daemon{false};
	bool shell{false};
	//uint16_t simul_num_nodes_deployed{0};
    string homedir;
    string sysophost{"127.0.0.1"};
};

void help() {
	params p;
	cout << "us-govd [options]" << endl;
	cout << "Options are:" << endl;
	cout << "  -d            Run daemon " << endl;
	cout << "  -ds           Run daemon with sysop shell" << endl;
	cout << "  -p <port>     Listening port, default " << p.port << endl;
	cout << "  -e <edges>    Max num neightbours, default " << p.edges << endl;
	cout << "  -home <homedir>   Set home directory (default is ~/.us) " << endl;
	cout << "  -host <address>   Specify where the daemon is running, defaults to localhost" << endl;
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
/*
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
*/
		if (inp=="-h") {
			help();
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

//RPC
struct thinfo {

	thinfo(const params& p): p(p), conf(cfg_id::load(p.homedir+"/gov")) {
	}
	const params& p;
	const cfg_id& conf;
	blockchain::peer_t* peer{0};

};

condition_variable cv22;
mutex mx22;
bool ready22{false};
bool finished22{false};
bool finished23{false};
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
        if (finished22) break;

		if (FD_ISSET (cli.sock, &read_fd_set)) {
			pair<string,datagram*> r=cli.recv();
            if (!r.first.empty()) {
                assert(r.second==0);
                cerr << r.first << endl;
                break;
            }
			if (demon.peerd.process_work_sysop(&cli,r.second)) {
				continue;
			}
			if (r.second->service!=us::gov::protocol::sysop) {
				delete r.second;
				continue;
			}
			cout << r.second->parse_string() << endl;
			delete r.second;
			{
			unique_lock<mutex> lock(mx22);
			ready22=true;
			}
			cv22.notify_all();
		}
	}
    finished23=true;
	cv22.notify_all();
}

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


	signal(SIGINT,sig_handler);
	signal(SIGTERM,sig_handler);
	signal(SIGPIPE, SIG_IGN);

//	us::gov::filesystem::cfg cfg=us::gov::filesystem::cfg::load(p.homedir);

	using us::gov::input::cfg_id;
	string homedir=p.homedir+"/gov";
	cfg_id conf=cfg_id::load(homedir);

	shell_client peer(conf.keys);
	if (!peer.connect(p.sysophost,p.port,true)) {
		cerr << "Cannot connect to " << p.sysophost << ":" << p.port << endl;
		return;
	}
	peer.run_auth();

	

//	shell_daemon d(cfg1::load(p.homedir).keys, p.listening_port, p.homedir, p.backend_host, p.backend_port);
//	d.run();


/*
	thinfo i(p);

	blockchain::peer_t cli(0);
	i.peer=&cli;
	
	cout << "us.gov Introspective Shell" << endl;
	cout << "Connecting to daemon at localhost:" << i.p.port << endl;
	if (!cli.connect(i.p.sysophost,i.p.port,true)) {
		cerr << "Cannot connect to " << i.p.sysophost << ":" << i.p.port << endl;
		return;
	}
	//cout << "Start thread" << endl;
	thread th(&shell_echo,&i);
	//cout << "/Start thread" << endl;
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
	while (!finished22) {
		cout << "> "; cout.flush();
		string line;
		getline(cin,line);
		if (line=="q") {
//			cout << ".." << endl;
			break;
		}
		datagram* d=new datagram(us::gov::protocol::sysop,line);
//cout << "sending " << us::gov::protocol::sysop << " " <<line << endl;
		cli.send(d);
		{
		unique_lock<mutex> lock(mx22);
		ready22=false;
		cv22.wait(lock,[]{return ready22;});
		}

	}
	cli.send(new datagram(protocol::ping));

	unique_lock<mutex> lock(mx22);
	cv22.wait(lock,[]{return finished23;});

	//this_thread::sleep_for(chrono::seconds(1));
	cli.disconnect();
	th.join();
	*/
}

void run_daemon(const params&p) {
	using us::gov::input::cfg_daemon;
	string homedir=p.homedir+"/gov";
	cfg_daemon conf=cfg_daemon::load(homedir);
	cout << "Node public key is " << conf.keys.pub << endl;
	signal(SIGINT,sig_handler);
	signal(SIGTERM,sig_handler);
	signal(SIGPIPE, SIG_IGN);
	blockchain::daemon d(conf.keys,conf.home,p.port,p.edges,conf.seed_nodes);
	d.sysop_allowed=p.shell;
	d.add(new cash::app());
	d.run();
	cout << "main: exited normally" << (thread_::_this.terminated?", terminated by signal":"") << endl;
}

#include <cstdlib>
#include <us/gov/crypto.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv) {

//    cfg::check_platform();

	params p;
	if(!parse_cmdline(argc,argv,p)) return 1;

    p.dump(cout);

	if (p.daemon) {
		run_daemon(p);
	}
	else { //RPC to govd - sysop
		open_shell(p);
	}
	return 0;
}

