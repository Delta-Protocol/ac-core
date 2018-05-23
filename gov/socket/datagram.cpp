#include "datagram.h"
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
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
#include <us/gov/net/os.h>
#include "server.h"

using namespace std;
using namespace us::gov::socket;

constexpr size_t datagram::h;
constexpr size_t datagram::maxsize;

datagram::datagram():error(0), dend(0) {
}

datagram::datagram(uint16_t service):service(service), error(0) {
	resize(h);
	encode_size(size());
	encode_service(service);
	dend=size();
}

datagram::datagram(uint16_t service, uint16_t payload):service(service), error(0) {
	resize(h+2);
	encode_size(size());
	encode_service(service);
	(*this)[h]=payload&0xff;
	(*this)[h+1]=payload>>8; //&0xff;
	dend=size();
}

datagram::datagram(uint16_t service, const string& payload):service(service), error(0) {
	assert(h+payload.size()<maxsize);
	resize(h+payload.size());
	encode_size(size());
	encode_service(service);
	::memcpy(&(*this)[h],payload.c_str(),payload.size());
	dend=size();
}

void datagram::encode_size(uint32_t sz) {
	assert(h==6);
	assert(size()>=h);
	(*this)[0]=sz&0xff;
	(*this)[1]=sz>>8&0xff;
	(*this)[2]=sz>>16&0xff;
	(*this)[3]=sz>>24&0xff;
}

uint32_t datagram::decode_size() const {
	assert(size()>3);
	uint32_t sz=(*this)[0];
	sz|=(*this)[1]<<8;
	sz|=(*this)[2]<<16;
	sz|=(*this)[3]<<24;
	return sz;
}

void datagram::encode_service(uint16_t svc) {
	assert(h==6);
	assert(size()>=h);
	(*this)[4]=svc&0xff;
	(*this)[5]=svc>>8&0xff;
}

uint16_t datagram::decode_service() const {
	assert(size()>5);
	uint16_t svc=(*this)[4];
	svc|=(*this)[5]<<8;
	return svc;
}

bool datagram::completed() const {
	return !empty() && dend==size();
}

bool datagram::recv(int sock, int timeout_seconds) {
	struct timeval tv;
	tv.tv_sec = timeout_seconds;
	tv.tv_usec = 0;
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
	return recv(sock);
}

bool datagram::recv(int sock) {
	if (dend<h) {
		if (size()<h) resize(h);
		ssize_t nread = server::os->recv(sock, &(*this)[dend], h-dend, 0);
		if (nread<=0) {
			if (errno==EINPROGRESS || errno==EAGAIN) { //https://stackoverflow.com/questions/2876024/linux-is-there-a-read-or-recv-from-socket-with-timeout
				cout << "socket: client: EINPROGRESS fd" << sock  << endl;
				error=4;
			}
			else {
				error=nread==0?1:2;
			}
			return false;
		}
		dend+=nread;
		if (dend<h) return true;
		uint32_t sz=decode_size();
		if (sz>maxsize) {
			error=3;
			return false;
		}
		resize(sz);
		service=decode_service();
		if (dend==sz) return true;
	}
	ssize_t nread = server::os->recv(sock, &(*this)[dend], size()-dend,0);
	if (nread<=0) {
		if (errno==EINPROGRESS || errno==EAGAIN) { //https://stackoverflow.com/questions/2876024/linux-is-there-a-read-or-recv-from-socket-with-timeout
			cout << "socket: client: EINPROGRESS fd" << sock  << endl;
			error=4;
		}
		else {
			error=nread==0?1:2;
		}
		return false;
	}
	dend+=nread;
	return true;
}

bool datagram::send(int sock) const {
	assert(size()<maxsize);
	uint8_t sz[h];
	auto nbytes = server::os->write(sock, &(*this)[0], size());
	if (nbytes < 0) {
		return false;
	}
	if (nbytes!=size()) {
		return false;
	}
	return true;
}

datagram::hash_t datagram::compute_hash() const {
	hasher_t hasher;
	hasher.write(reinterpret_cast<const unsigned char*>(&*begin()),size());
	hasher_t::value_type v;
	hasher.finalize(v);
	return move(v);
}

vector<string> datagram::parse_strings() const {
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

string datagram::parse_string() const {
	return string(reinterpret_cast<const char*>(&*(begin()+h)),size()-h);
}

uint16_t datagram::parse_uint16() const {
	if (size()!=h+2) return 0;
	uint16_t pl=(*this)[h];
	pl|=(*this)[h+1]<<8;
	return pl;
}

