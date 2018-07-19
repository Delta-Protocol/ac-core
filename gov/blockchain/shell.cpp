#include "shell.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <us/gov/auth.h>
#include <chrono>
#include <cassert>
#include <sstream>
#include <vector>
#include <us/gov/signal_handler.h>
#include "daemon.h"

using namespace us::gov::blockchain;
typedef us::gov::blockchain::shell c;
using namespace std;


void c::help(ostream& os) const {
/*
os << "us.gov" << endl;
os << "======" << endl;
os << "" << endl;
os << "1.Users" << endl;
os << "  1.1. Run a node" << endl;
os << "2.Developers" << endl;
os << "  2.1. Clone sources" << endl;
os << "  2.2. Submit changes" << endl;
os << "" << endl;
os << "" << endl;
os << "" << endl;
os << "1.1. Run a node" << endl;
os << "" << endl;
os << "" << endl;
os << "2.1. clone sources" << endl;
os << "" << endl;
os << "" << endl;
os << "2.2. submit changes" << endl;

os << "" << endl;
*/
os << "This is Free Software licensed under GNU Affero General Public Licence (AGPL). " << endl;
os << "Obtain a copy of the licence here: https://www.gnu.org/licenses/agpl.txt" << endl;

	os << "us.gov ; Introspective Shell. Node id " << d.id.pub << endl;
	os << "h              Shows this help." << endl;
	os << "l, level [int]    Change level. Current level is " << level << endl;
	os << "      " << (level==0?'>':' ') << "level 0 - socket" << endl;
	os << "      " << (level==1?'>':' ') << "level 1 - id" << endl;
	os << "      " << (level==2?'>':' ') << "level 2 - auth" << endl;
	os << "      " << (level==3?'>':' ') << "level 3 - peer" << endl;
	os << "      " << (level==4?'>':' ') << "level 4 - relay" << endl;
	os << "      " << (level==5?'>':' ') << "level 5 - dfs" << endl;
	os << "      " << (level==6?'>':' ') << "level 6 - blockchain" << endl;
	if (level==0) {
		os << "Level " << level << " (socket) help:" << endl;
		os << "s|server    Dumps daemon info." << endl;
	}
	else if (level==1) {
		os << "Level " << level << " (id) help:" << endl;
		os << "s|server    Dumps daemon info." << endl;
	}
	else if (level==2) {
		os << "Level " << level << " (auth) help:" << endl;
		os << "s|server    Dumps daemon info." << endl;
	}
	else if (level==3) {
		os << "Level " << level << " (peer) help:" << endl;
		os << "s|server    Dumps daemon info." << endl;
	}
	else if (level==4) {
		os << "Level " << level << " (relay) help:" << endl;
		os << "s|server    Dumps daemon info." << endl;
	}
	else if (level==5) {
		os << "Level " << level << " (dfs) help:" << endl;
		os << "s|server    Dumps daemon info." << endl;
	}
	else if (level==6) { //change init level to this in shell.h
		os << "Level " << level << " (blockchain) help:" << endl;
		os << "s|server    Dumps networking info." << endl;
		os << "y|syncd     Dumps data sync info." << endl;
		os << "apps        List apps." << endl;
		os << "app <id>    Enter app shell." << endl;
		os << "add_node <address>    Connects to the specified node." << endl;
		os << "seeds                 Display current seed nodes." << endl;
		os << "performances          Print performances" << endl;
		os << "home                  Display current home directory." << endl;
		os << "mutate                Renew neighbours." << endl;

	}
	else {
		os << "No help page for level " << level << endl;
	}

}


string c::command(const string& cmdline) {
	ostringstream os;
	istringstream is(cmdline);
	string cmd;
	is >> cmd;

	if (cur_app!=-1) {
		auto s=d.shell_command(cur_app,cmdline);
		if (s.empty()) {
			os << "exited app shell" << endl;
			cur_app=-1;
			return os.str();
		}
		return s;
	}

	if (cmd=="h") {
		help(os);
		memory_cmd_lvl.clear(); //do nothing if level changes
	}
	else if (cmd=="level" || cmd=="l") {
		int i=-1;
		is >> i;
		if (i>=0) level=i;
		os << "Current level is: " << level << endl;
		if (!memory_cmd_lvl.empty()) command(memory_cmd_lvl);
	}
	else if (cmd=="server" || cmd=="s") {
		switch(level) {
		case 0:
			dynamic_cast<const us::gov::socket::server&>(d.peerd).dump(os); break;
		case 1:
			dynamic_cast<const us::gov::id::daemon&>(d.peerd).dump(os); break;
		case 2:
			dynamic_cast<const us::gov::auth::daemon&>(d.peerd).dump(os); break;
		case 3:
			dynamic_cast<const us::gov::peer::daemon&>(d.peerd).dump(os); break;
		case 4:
			dynamic_cast<const us::gov::relay::daemon&>(d.peerd).dump(os); break;
		case 5:
			dynamic_cast<const us::gov::dfs::daemon&>(d.peerd).dump(os); break;
		case 6:
			d.peerd.dump(os); break;
		default:
			os << "No daemon at level " << level << endl;
		}
		memory_cmd_lvl=cmd;
	}
	else if (cmd=="syncd" || cmd=="y") {
		d.syncdemon.dump(os);
	}
	else if (cmd=="lu") {
		level++;
		if (!memory_cmd_lvl.empty()) command(memory_cmd_lvl);
	}
	else if (cmd=="ld") {
		if (level>0) level--;
		if (!memory_cmd_lvl.empty()) command(memory_cmd_lvl);
	}
	else if (cmd=="apps") {
		d.list_apps(os);
	}
	else if (cmd=="app") {
		int id=-1;
		is >> id;
		if (id<0) {
			os << "Missing argument app id" << endl;
		}
		else {
			cur_app=id;
			os << d.shell_command(cur_app,"hello");
		}
	}
    else if (cmd=="add_node") {
        string ip;
        is >> ip;
        if (!ip.empty()) {
            d.peerd.seed_nodes.push_back(ip);
    		os << "Added seed node " << ip << endl;
        }
        else {
    		os << "Missing address" << endl;
        }
    }
    else if (cmd=="seeds") {
   		os << "Seed nodes" << endl;
        for (auto& i:d.peerd.seed_nodes) {
    		os << i << endl;
        }
    }
    else if (cmd=="performances") {
	d.print_performances(os);
    }
    else if (cmd=="home") {
   		os << d.home << endl;
    }
    else if (cmd=="mutate") {
        d.peerd.daemon_timer();
   		os << "mutation invoked" << endl;
    }

	else {
		memory_cmd_lvl.clear(); //do nothing if level changes
		os << "Unknown command '" << cmd << "'" << endl;
	}
	return os.str();
}




