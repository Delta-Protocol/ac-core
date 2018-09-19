#include "daemon.h"
#include "protocol.h"

#include <filesystem>
#include <fstream>

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

    ifstream is(homedir+"/"+hash_b58);
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
    string filename=homedir+"/"+hash_b58;
    if (unlikely(!fs::exists(filename))) {
        {
            ofstream os(filename);
            os << content;
        }

        auto it = file_cv.find(hash_b58);
        if(unlikely(it != end(file_cv))) {
            it->second->notify_all();
            file_cv.erase(it);
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

void c::dump(ostream&os) const {
    os << "Hello from dfs::daemon" << endl;
    os << "Nothing to say here yet" << endl;    
}

