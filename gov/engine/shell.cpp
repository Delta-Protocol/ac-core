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

using namespace us::gov::engine;
using namespace std;


void shell::help(ostream& os) const {

    os << "This is Free Software licensed under GNU Affero General Public Licence (AGPL). " << endl;
    os << "Obtain a copy of the licence here: https://www.gnu.org/licenses/agpl.txt" << endl;

    os << "us.gov ; Introspective Shell. Node id " << m_d.get_id().get_pubkey() << endl;
    os << "h              Shows this help." << endl;
    os << "l, level [int]    Change level. Current level is " << m_level << endl;
    os << "      " << (m_level==0?'>':' ') << "level 0 - socket server" << endl;
    os << "      " << (m_level==1?'>':' ') << "level 1 - socket daemon" << endl;
    os << "      " << (m_level==2?'>':' ') << "level 2 - id" << endl;
    os << "      " << (m_level==3?'>':' ') << "level 3 - auth" << endl;
    os << "      " << (m_level==4?'>':' ') << "level 4 - peer" << endl;
    os << "      " << (m_level==5?'>':' ') << "level 5 - relay" << endl;
    os << "      " << (m_level==6?'>':' ') << "level 6 - dfs" << endl;
    os << "      " << (m_level==7?'>':' ') << "level 7 - engine" << endl;
    if (m_level==0) {
        os << "Level " << m_level << " (socket server) help:" << endl;
        os << "s|server    Dumps server info." << endl;
    }else if (m_level==1) {
        os << "Level " << m_level << " (socket daemon) help:" << endl;
        os << "s|server    Dumps daemon info." << endl;
    }else if (m_level==2) {
        os << "Level " << m_level << " (id) help:" << endl;
        os << "s|server    Dumps daemon info." << endl;
    }else if (m_level==3) {
        os << "Level " << m_level << " (auth) help:" << endl;
        os << "s|server    Dumps daemon info." << endl;
    }else if (m_level==4) {
        os << "Level " << m_level << " (peer) help:" << endl;
        os << "s|server    Dumps daemon info." << endl;
    }else if (m_level==5) {
        os << "Level " << m_level << " (relay) help:" << endl;
        os << "s|server    Dumps daemon info." << endl;
    }else if (m_level==6) {
        os << "Level " << m_level << " (dfs) help:" << endl;
        os << "s|server    Dumps daemon info." << endl;
    }else if (m_level==7) { //change init level to this in shell.h
        os << "Level " << m_level << " (engine) help:" << endl;
        os << "s|server    Dumps networking info." << endl;
        os << "y|syncd     Dumps data sync info." << endl;
        os << "apps        List apps." << endl;
        os << "app <id>    Enter app shell." << endl;
        os << "add_node <address>    Connects to the specified node." << endl;
        os << "seeds                 Display current seed nodes." << endl;
        os << "performances          Print performances" << endl;
        os << "home                  Display current home directory." << endl;
        os << "mutate                Renew neighbours." << endl;
    }else {
        os << "No help page for level " << m_level << endl;
    }
}

string shell::command(const string& cmdline) {
    ostringstream os;
    istringstream is(cmdline);
    string cmd;
    is >> cmd;

    if (m_cur_app!=-1) {
        auto s = m_d.shell_command(m_cur_app,cmdline);
        if (s.empty()) {
            os << "exited app shell" << endl;
            m_cur_app=-1;
            return os.str();
        }
        return s;
    }

    if (cmd=="h") {
        help(os);
        m_memory_cmd_lvl.clear(); 
    }else if (cmd=="level" || cmd=="l") {
        int i=-1;
        is >> i;
        if (i>=0) 
            m_level=i;
        os << "Current level is: " << m_level << endl;
        if (!m_memory_cmd_lvl.empty()) 
            command(m_memory_cmd_lvl);
    }else if (cmd=="server" || cmd=="s") {
        switch(m_level) {
            case 0:
                dynamic_cast<const us::gov::socket::server&>(m_d.get_peerd()).dump(os); 
                break;
            case 1:
                dynamic_cast<const us::gov::socket::daemon&>(m_d.get_peerd()).dump(os); 
                break;
            case 2:
                dynamic_cast<const us::gov::id::daemon&>(m_d.get_peerd()).dump(os); 
                break;
            case 3:
                dynamic_cast<const us::gov::auth::daemon&>(m_d.get_peerd()).dump(os); 
                break;
            case 4:
                dynamic_cast<const us::gov::peer::daemon&>(m_d.get_peerd()).dump(os); 
                break;
            case 5:
                dynamic_cast<const us::gov::relay::daemon&>(m_d.get_peerd()).dump(os); 
                break;
            case 6:
                dynamic_cast<const us::gov::dfs::daemon&>(m_d.get_peerd()).dump(os); 
                break;
            case 7:
                m_d.get_peerd().dump(os); 
                break;
            default:
                os << "No daemon at level " << m_level << endl;
        }
        m_memory_cmd_lvl=cmd;
    }else if (cmd=="syncd" || cmd=="y") {
        m_d.get_syncd().dump(os);
    }else if (cmd=="lu") {
        m_level++;
        if (!m_memory_cmd_lvl.empty()) 
            command(m_memory_cmd_lvl);
    }else if (cmd=="ld") {
        if (m_level>0) 
            m_level--;
        if (!m_memory_cmd_lvl.empty()) 
            command(m_memory_cmd_lvl);
	}else if (cmd=="apps") {
            m_d.list_apps(os);
	}else if (cmd=="app") {
            int m_id=-1;
            is >> m_id;
            if (m_id<0) {
                os << "Missing argument app id" << endl;
            }else {
                m_cur_app=m_id;
                os << m_d.shell_command(m_cur_app,"hello");
            }
        }else if (cmd=="add_node") {
            string ip;
            is >> ip;
            if (!ip.empty()) {
                m_d.add_ip_to_seed_nodes(ip);//.seed_nodes.push_back(ip);
                os << "Added seed node " << ip << endl;
            }
            else {
                os << "Missing address" << endl;
            }
        }else if (cmd=="seeds") {
            os << "Seed nodes" << endl;
            for (const auto& i:m_d.get_peerd().get_seed_nodes()) {
                os << i << endl;
            }
        }else if (cmd=="performances") {
            m_d.print_performances(os);
        }else if (cmd=="home") {
            os << m_d.get_home() << endl;
        }else if (cmd=="mutate") {
            m_d.peerd_daemon_timer();
            os << "mutation invoked" << endl;
        }else {
            m_memory_cmd_lvl.clear();
            os << "Unknown command '" << cmd << "'" << endl;
        }
    return os.str();
}




