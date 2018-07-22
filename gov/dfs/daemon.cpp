#include "daemon.h"
#include "protocol.h"

using namespace us::gov;
using namespace us::gov::dfs;
typedef us::gov::dfs::daemon c;

bool c::process_work(socket::peer_t *c, datagram*d) {
//cout << "dfs::process_work for " << d->service << endl;
	if (b::process_work(c,d)) return true;
	peer_t* p=static_cast<peer_t*>(c);
	switch(d->service) {
		    case protocol::file_request: receive_file(p,d); break;
		    case protocol::file_response: send_file(p,d); break;
		    default: return false;
	}
	return true;
}

void c::receive_file(peer_t* c, datagram* d) {
	cout << "dfs: receivd file from " << c->addr << endl;
	string hash;
	vector<uint8_t> data;
	save(hash,data,-1);
	delete d;
}

void c::send_file(peer_t* c, datagram* request) {
	cout << "dfs: sending requested file to " << c->addr << endl;
	bool found=false;
	if (found) {
		vector<uint8_t> data;
		datagram*d=new datagram(protocol::file_response,move(data));
		c->send(d);
		delete request;
	}
	else {
		send(1,c,request);
	}
}

void c::save(const string& hash, const vector<uint8_t>& data, int propagate) { //-1 nopes, 0=all peers; n num of peers
	cout << "dfs: save file " << hash << " propagate=" << propagate << endl;

	if (propagate==-1) return;
	
	//if file already exists return
	//save with tmpname
	//rename atomically
}

string c::load(const string& hash) {
	cout << "dfs: load file " << hash << endl;
	string filename;
	bool found=false;
	if (found) return filename;
	cout << "dfs: not found locally" << endl;

	datagram* d=new datagram(protocol::file_request,hash);
	send(1,0,d);
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

