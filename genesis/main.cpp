#include <us/gov/auth.h>
#include <us/gov/engine.h>
#include <us/gov/cash.h>
#include <us/gov/signal_handler.h>
#include <us/gov/socket/datagram.h>
#include <us/gov/engine/protocol.h>

using namespace us::gov;
using namespace std;
using socket::datagram;

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
    }

    string homedir;
};

void help() {
	params p;
	cout << "us-genesis [options] <WAN address>    The IP address other nodes will use to reach this node." << endl;
	cout << "Options are:" << endl;
	cout << "  -home <homedir>   Set home directory (default is ~/.us) " << endl;
	cout << "  -h            Print this help and exit " << endl;
}

#include <us/gov/input.h>
using us::gov::input::shell_args;
using namespace us;

string parse_options(shell_args& args, params& p) {
    string cmd;
    while(true) {
        cmd=args.next<string>();
        if (cmd=="-home") {
            p.homedir=args.next<string>();
        }
		else if (cmd=="-h") {
			help();
			exit(0);
		}
        else {
            break;
        }
    }
    return cmd;
}

#include <us/gov/crypto.h>
#include <string.h>
#include <iomanip>
#include <fstream>
#include <us/gov/input.h>
#include <us/gov/engine/diff.h>

using us::gov::engine::diff;

struct genesis_daemon: engine::daemon {

    genesis_daemon(const us::gov::crypto::ec::keys& a, const string& b):engine::daemon(a,b,0,0,vector<string>()) {
	    add(new cash::app());
    }

    void start_new_blockchain(const string& addr) {
        assert(!addr.empty());
        auto pool=new diff();
        auth_app->pool->to_hall.push_back(make_pair(id.pub.hash(),addr));
        auto* mg=create_local_deltas();
        assert(mg!=0);
        pool->allow(*mg);
        pool->add(mg);
        pool->end_adding();
        save(*pool);
        if (!import(*pool)) {
            cerr << "Error creating blockchain" << endl;
            exit(1);
        }
        delete pool;
    }

};


using us::gov::input::cfg;
using us::gov::input::cfg1;

int main(int argc, char** argv) {

    shell_args args(argc,argv);
    params p;
    string address=parse_options(args,p);

//	params p;
//	if(!parse_cmdline(argc,argv,p)) return 1;

    p.dump(cout);

    if (address.empty()) {
			cerr << "I need the address of the genesis node." << endl;
            help();
			exit(1);
    }

	if (!cfg::ensure_dir(p.homedir)) {
		cerr << "Cannot create dir " << p.homedir << endl;
		exit(1);
	}

	p.homedir+="/gov";

	if (cfg::exists_dir(p.homedir)) {
		cerr << "Cannot start a new blockchain if home dir exists: " << p.homedir << endl;
		exit(1);
	}

	cfg conf=cfg::load(p.homedir);

	if (!conf.keys.pub.valid) {
		cerr << "Invalid node pubkey" << endl;
		exit(1);
	}

        cout << "Node public key is " << conf.keys.pub << " address " << conf.keys.pub.hash() << endl;
	genesis_daemon d(conf.keys,conf.home); //,0,0,vector<string>());

//    cout << "Adding genesis block with 1 node " << p.genesis_address << endl;
    d.start_new_blockchain(address);
    cout << "New blockchain has been created. home is " << p.homedir << endl;
	return 0;
}

