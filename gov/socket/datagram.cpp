/** @file */
#include "datagram.h"
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <us/gov/likely.h>
#include <stdio.h>
#include <errno.h>
#include <cassert>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "server.h"

using namespace std;
using namespace us::gov::socket;
typedef us::gov::socket::datagram c;

constexpr size_t c::h;
constexpr size_t c::maxsize;

struct kk {
int a;
};

c::datagram(): dend(0) {
}

c::datagram(uint16_t service):service(service) {
	resize(h);
	encode_size(size());
	encode_service(service);
	dend=size();
}

c::datagram(uint16_t service, uint16_t payload):service(service) {
	resize(h+2);
	encode_size(size());
	encode_service(service);
	(*this)[h]=payload&0xff;
	(*this)[h+1]=payload>>8; //&0xff;
	dend=size();
}

c::datagram(uint16_t service, const string& payload):service(service) {
	assert(h+payload.size()<maxsize);
	resize(h+payload.size());
	encode_size(size());
	encode_service(service);
	::memcpy(&(*this)[h],payload.c_str(),payload.size()); //no trailing zro
	dend=size();
}

c::datagram(uint16_t service, vector<uint8_t>&&) {
assert(false);
}

void c::encode_size(uint32_t sz) {
	assert(h==6);
	assert(size()>=h); //This is little-endian
	(*this)[0]=sz&0xff;
	(*this)[1]=sz>>8&0xff;
	(*this)[2]=sz>>16&0xff;
	(*this)[3]=sz>>24&0xff;
}

uint32_t c::decode_size() const {
	assert(size()>3);
	uint32_t sz=(*this)[0];
	sz|=(*this)[1]<<8;
	sz|=(*this)[2]<<16;
	sz|=(*this)[3]<<24;
	return sz;
}

void c::encode_service(uint16_t svc) {
	assert(h==6);
	assert(size()>=h);
	(*this)[4]=svc&0xff;
	(*this)[5]=svc>>8&0xff;
}

uint16_t c::decode_service() const {
	assert(size()>5);
	uint16_t svc=(*this)[4];
	svc|=(*this)[5]<<8;
	return svc;
}

bool c::completed() const {
	return dend==size() && !empty();
}

string c::recvfrom(int sock) {
	if (unlikely(sock==0)) {
            return "Error. Connection is closed.";
        }
	if (dend<h) {
		if (size()<h) resize(h);
		ssize_t nread = ::recv(sock, &(*this)[dend], h-dend, 0);
		if (unlikely(nread<=0)) {
			if (errno==EINPROGRESS || errno==EAGAIN) { //https://stackoverflow.com/questions/2876024/linux-is-there-a-read-or-recv-from-socket-with-timeout
    			    return "Timeout waiting for data from peer.";
			}
			else {
    			    return "Connection ended by peer.";
			}
		}
		dend+=nread;
		if (dend<h) {
                     return ""; //need to recv more
                }
		uint32_t sz=decode_size();
		if (sz>maxsize) {
			return "Error. Incoming datagram is too big.";
		}
		resize(sz);
		service=decode_service();
		if (dend==sz) {
            return "";
        }
	}
	ssize_t nread = ::recv(sock, &(*this)[dend], size()-dend,0);
	if (nread<=0) {
		if (errno==EINPROGRESS || errno==EAGAIN) { //https://stackoverflow.com/questions/2876024/linux-is-there-a-read-or-recv-from-socket-with-timeout
   			return "Error.3 Timeout waiting for data from peer.";
		}
		else {
			return "Error. Incoming datagram is too big.";
		}
	}
	dend+=nread;
	return "";
}

string c::sendto(int sock) const {
	if (unlikely(size()>=maxsize)) {
            return "Error. Datagram is too big.";
        }
	if (unlikely(sock==0)) {
            return "Error. Connection is closed.";
        }
	uint8_t sz[h];
	auto n = ::write(sock, &(*this)[0], size());
	if (unlikely(n<0)) {
		return "Error. Failure writting to socket.";
	}
	if (unlikely(n!=size())) {
		return "Error. Unexpected returning size while wrtting to socket";
	}
	return "";
}

c::hash_t c::compute_hash() const {
	hasher_t hasher;
	hasher.write(reinterpret_cast<const unsigned char*>(&*begin()),size());
	hasher_t::value_type v;
	hasher.finalize(v);
	return move(v);
}
/*
vector<string> c::parse_strings() const {
	vector<string> ans;
	ans.reserve(100);
	const unsigned char *s=0;
	size_t sz=0;
	for (auto i=begin()+h; i!=end(); ++i) {
		if (!s) s=static_cast<const unsigned char*>(&*i);
		if (*i=='\n') {
			ans.emplace_back(string(reinterpret_cast<const char*>(s),sz));
			sz=0;
			s=0;
		}
		else ++sz;
	}
	if (s) {
		ans.emplace_back(string(reinterpret_cast<const char*>(s),sz));
	}
	return move(ans);
}
*/
#include <us/gov/id/protocol.h>
#include <us/gov/dfs/protocol.h>
#include <sstream>
string c::parse_string() const {
#ifdef DEBUG
if (service==protocol::gov_id_peer_status) {
assert(false);
    ostringstream os;
    os << "(uint16)" << parse_uint16();
    return os.str();
}
else if (service==protocol::file_response) {
assert(false);
    return "[binary]";
}
else {
#endif
    int sz=size()-h+1;
    if (unlikely(sz<2)) return ""; //minimum 2bytes for a non-empty c string
    char cstr[sz];
    memcpy(cstr,&*(cbegin()+h),sz-1);
    cstr[sz-1]='\0';
    return string(cstr);
#ifdef DEBUG
}
#endif
//	return string(reinterpret_cast<const char*>(&*(begin()+h)),size()-h);
}

uint16_t c::parse_uint16() const {
	if (size()!=h+2) return 0;
	uint16_t pl=(*this)[h];
	pl|=(*this)[h+1]<<8;
	return pl;
}

void c::dump(ostream&os) const {
os << "size " << size() << endl;
os << "service " << decode_service() << endl;
os << "payload size " << decode_size() << endl;
os << "payload as str " << parse_string() << endl;
os << "completed " << completed() << endl;
}

#ifdef DEBUG
#include <sstream>
string c::service_str(uint16_t svc) {
    static constexpr const char *x{"PEQ_"};
    ostringstream os;
    if (svc>=100) {
        os << x[svc&3] << ".";
        auto m=svc>>2;
        uint16_t l=m/100;
        os << l << "." << m%100;
    }
    else {
        os << "wallet";
    }
    return os.str();
}
#endif
