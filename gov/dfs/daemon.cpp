#include "daemon.h"
#include "protocol.h"

#include <filesystem>
#include <fstream>

using namespace std::chrono;
namespace fs = std::filesystem;

using namespace us::gov;
using namespace us::gov::dfs;
typedef us::gov::dfs::daemon c;

bool c::process_work(socket::peer_t *c, datagram*d) {
	if (b::process_work(c,d)) return true;
	peer_t* p=static_cast<peer_t*>(c);
	switch(d->service) {
		    case protocol::file_request: request(p,d); break;
		    case protocol::file_response: response(p,d); break;
		    default: return false;
    }
	return true;
}

void c::request(peer_t *c, datagram*d) {
    auto hash_b58=d->parse_string();
    delete d;

    ifstream is(get_path_from(hash_b58));
    if (!is.good()) {
        cout << "block not found in HD, ignoring." << endl;
        return;
    }

    string content=string((istreambuf_iterator<char>(is)), (istreambuf_iterator<char>()));
    c->send(new datagram(protocol::file_request,content));
}

void c::response(peer_t *c, datagram*d) {
    auto content=d->parse_string();
    delete d;

    auto hash_b58 = d->compute_payload_hash().to_b58();
    string filename=get_path_from(hash_b58,true);
    if (unlikely(!fs::exists(filename))) {
        {
            ofstream os(filename);
            os << content;
        }

        file_cv.notify_and_erase(hash_b58);
    }

    file_cv.purge();
}

void c::file_cv_t::notify_and_erase(const string& hash_b58) {
    lock_guard<mutex> lock(mx);
    auto it = this->find(hash_b58);
    if(unlikely(it != this->end())) {
        it->second.first->notify_all();
        this->erase(it);
    }
}

void c::file_cv_t::add(const string& hash_b58, condition_variable* pcv) {
    lock_guard<mutex> lock(mx);
    assert(pcv != 0);
    auto now = system_clock::now();
    this->emplace(hash_b58, make_pair(pcv,now));
}

void c::file_cv_t::purge() {
    lock_guard<mutex> lock(mx);
    auto it = this->begin();
    auto now = system_clock::now();
    while(it != this->end()) {
        auto elapsed = duration_cast<seconds>(now-it->second.second);
        if(elapsed>60s) {
            it = this->erase(it);
        } else {
            it++;
        }
    }
}

void c::save(const string& hash, const vector<uint8_t>& data, int propagate) { //-1 nopes, 0=all peers; n num of peers
assert(false);
/*
	cout << "dfs: save file " << hash << " propagate=" << propagate << endl;

	if (propagate==-1) return;
	
	//if file already exists return
	//save with tmpname
	//rename atomically
*/
}

string c::load(const string& hash) {
assert(false);
/*
	cout << "dfs: load file " << hash << endl;
	string filename;
	bool found=false;
	if (found) return filename;
	cout << "dfs: not found locally" << endl;

	datagram* d=new datagram(protocol::file_request,hash);
	send(1,0,d);
*/
}

string c::resolve_filename(const string& filename) {
	
	string res;
	
	int max_length = filename.size()/2 +filename.size(); //final string length with slashes
	res.reserve(max_length);
    for(int i=0; i < filename.size(); i++){// slash"/" every 2 char
        if((i&1) == 0){ 
			res+="/";
        }
		res+=filename[i];
    }
	
	if(res[res.size()-1]=='/'){
		res.end()-1;//delete the last slash '/'
	}
	return &res[1];
}

string c::get_path_from(const string& hash_b58, bool create_dirs) const {
    auto file_path=homedir +"/"+resolve_filename(hash_b58);

    if(create_dirs) {
        fs::path dir(file_path);

        auto p=dir.parent_path();
        if(!(fs::exists(p))) {
            if(!fs::create_directories(p))
                return "";
        }
    }

    return file_path;
}

void c::dump(ostream&os) const {
    os << "Hello from dfs::daemon" << endl;
    os << "Nothing to say here yet" << endl;    
}

