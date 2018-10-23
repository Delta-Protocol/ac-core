#include "daemon.h"
#include "protocol.h"
#include <us/gov/engine/diff.h>
#include <us/gov/stacktrace.h>
#include <filesystem>
#include <fstream>

using namespace std::chrono;
namespace fs = std::filesystem;

using namespace us::gov;
using namespace us::gov::dfs;

bool daemon::process_work(socket::peer_t *c, socket::datagram*d) {
    if (relay::daemon::process_work(c,d))
        return true;
    relay::peer_t* p=static_cast<relay::peer_t*>(c);
    switch(d->service) {
        case protocol::file_request: 
             request(p,d); 
             break;
	case protocol::file_response: 
             response(p,d); 
             break;
	default: 
             return false;
    }
	return true;
}

void daemon::request(relay::peer_t *c, socket::datagram*d) {
    auto  hash_b58=d->parse_string();
    delete d;
    cout << "received request for file " << hash_b58 << endl;

    ifstream is(get_path_from(hash_b58));
    if (!is.good()) {
        auto n=this->get_random_edge(c);
        if (unlikely(n==0))
            return;
        cout << "block not found in HD, requesting it from peer " << n->m_addr << endl;
        auto r=n->send(new socket::datagram(protocol::file_request,hash_b58));
        cout << r << endl;
        return;
    }

    cout << "sending  file " << hash_b58 << endl;
    string content=string((istreambuf_iterator<char>(is)), (istreambuf_iterator<char>()));
    c->send(new socket::datagram(protocol::file_response,content));
}

void daemon::response(relay::peer_t *c, socket::datagram*d) {
    auto hash_b58 = d->compute_payload_hash().to_b58();

    if(!m_file_cv.exists(hash_b58)) {
        if(!m_recents.exists(hash_b58)) {
            cout << "DFS - disconnecting peer because of file: " << hash_b58 << endl;
            c->disconnect();
        }
    }

    string filename=get_path_from(hash_b58,true);
    if (unlikely(!fs::exists(filename))) {
        {
            ofstream os(filename);
            auto content=d->parse_string();
            os << content;
        }

        m_file_cv.notify_and_erase(hash_b58);
    }
    delete d;
}

void daemon::daemon_timer() {
    relay::daemon::daemon_timer();
    cout << "DFS housekeeping" << endl;
    m_file_cv.purge(); 
    m_recents.purge();
}

void daemon::file_cv_t::notify_and_erase(const string& hash_b58) {
    lock_guard<mutex> lock(mx);
    auto it = this->find(hash_b58);

    if(unlikely(it != this->end())) {
        assert(it->second.pcv != 0);
        it->second.file_arrived=true;
        it->second.pcv->notify_all();
        delete it->second.pcv;
        this->erase(it);
    }
}

void daemon::file_cv_t::add(const string& hash_b58, condition_variable * pcv, bool file_arrived) {
    lock_guard<mutex> lock(mx);
    assert(pcv!=0);
    auto now = system_clock::now();
    this->emplace(hash_b58, daemon::params({pcv,now,file_arrived}));
}

void daemon::file_cv_t::purge() {
    lock_guard<mutex> lock(mx);
    auto it = this->begin();
    auto now = system_clock::now();

    while(it != this->end()) {
        auto elapsed = duration_cast<seconds>(now-it->second.time);
        if(elapsed>60s) {
            delete it->second.pcv;
            it = this->erase(it);
        } else {
            it++;
        }
    }
}

bool daemon::file_cv_t::exists(const string& hash_b58) {
    lock_guard<mutex> lock(mx);
    return this->find(hash_b58) != this->end();
}

void daemon::file_cv_t::wait_for(const string& hash_b58) {
    unique_lock<mutex> lock(mx);
    auto it = find(hash_b58);
    if(it !=  this->end()) {
        it->second.pcv->wait_for(lock, 3s, 
                    [&]{ return it->second.file_arrived || 
                                program::_this.terminated; });
        it->second.file_arrived = false;
    }
}

void daemon::file_cv_t::remove(const string& hash_b58) {
    unique_lock<mutex> lock(mx);
    auto it = find(hash_b58);
    if(it != this->end()) {
        delete it->second.pcv;
        this->erase(it);
    }
}

void daemon::recents_t::add(const string& hash_b58) {
    lock_guard<mutex> lock(mx);
    auto now = system_clock::now();
    this->emplace(hash_b58, now);
}

bool daemon::recents_t::exists(const string& hash_b58) {
    lock_guard<mutex> lock(mx);
    return this->find(hash_b58) != this->end();
}

void daemon::recents_t::purge() {
    lock_guard<mutex> lock(mx);
    auto it = this->begin();
    auto now = system_clock::now();

    while(it != this->end()) {
        auto elapsed = duration_cast<seconds>(now-it->second);
        if(elapsed>60s) {
            it = this->erase(it);
        } else {
            it++;
        }
    }
}

string daemon::load(const string& hash_b58, 
                    condition_variable * pcv, bool file_arrived) {
    auto filename = m_homedir +"/"+resolve_filename(hash_b58);
    if(fs::exists(filename)) {
        return filename;
    } else {
        m_file_cv.add(hash_b58,pcv,file_arrived);
        auto n=this->get_random_edge();
        if (unlikely(n==0))
        	return "";
        cout << "DFS: querying file for " << hash_b58
             << " from peer " << n->m_addr << endl;
        auto r=n->send(new socket::datagram(protocol::file_request,hash_b58));
        cout << r << endl;
        return "";
    }
}

string daemon::load(const string& hash_b58) {
    string filename;
    if(!m_file_cv.exists(hash_b58)) {
        condition_variable * pcv = new condition_variable;
        filename=load(hash_b58, pcv, false);
    }
    if(filename.empty()) m_file_cv.wait_for(hash_b58);

    m_file_cv.remove(hash_b58);
    m_recents.add(hash_b58);  

    return filename;
}


bool daemon::exists(const string& hash_b58) const {
    return fs::exists(get_path_from(hash_b58,false));
}

void daemon::save(const string& hash_b58, const string& data) {
    ostringstream filename;
    filename << get_path_from(hash_b58,true);

    if(!fs::exists(filename.str())) {
        ofstream os(filename.str());
        os << data;
    }
}

string daemon::resolve_filename(const string& filename) {
	
    string res;
	
    int max_length = filename.size()/2 +filename.size(); 
    res.reserve(max_length);
    for(int i=0; i < filename.size(); i++){
        if((i&1) == 0){
            res+="/";
        }
        res+=filename[i];
    }
	
    if(res[res.size()-1]=='/'){
        res.end()-1;
    }
    return &res[1];
}

string daemon::get_path_from(const string& hash_b58, 
                             bool create_dirs) const {
    auto file_path = m_homedir +"/"+resolve_filename(hash_b58);

    if(create_dirs) {
        fs::path dir(file_path);

        auto p=dir.parent_path();
        if(!(fs::exists(p))) {
            assert(fs::create_directories(p));
        }
    }

    return file_path;
}

bool daemon::remove(const string& hash_b58) {
    auto sub  = "/" + resolve_filename(hash_b58);
    auto path = m_homedir + sub;

    bool deleted = std::remove(path.c_str());
    if(deleted != 0) return false;

    string tmp;
    for(auto i=sub.length()-1; i; i--) {
        if(sub[i] == '/') {
            tmp = path.substr(0,m_homedir.length() + i);

            if(!fs::exists(tmp)) {
                cerr << "Filesystem inconsistent - folder " 
                     << tmp << " does not exists" << endl;
                return false;
            }

            if (fs::is_empty(tmp)) {
                std::remove(tmp.c_str());
            } else {
                break; 
            }

            tmp="";
        }
    }
    return true;
}

void daemon::dump(ostream&os) const {
    os << "Hello from dfs::daemon" << endl;
    os << "Nothing to say here yet" << endl;    
}

