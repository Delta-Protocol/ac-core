#include "client.h"

#include <asm-generic/errno.h>
#include <asm-generic/socket.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <sstream>
#include <thread>
#include <utility>

#include "us/gov/signal_handler.h"
#include "us/gov/likely.h"
#include "protocol.h"

using namespace std;
using namespace us::gov::socket;

client::client():m_sock(0) {
}

client::client(int sock):m_sock(sock) {
    if (sock!=0) m_addr=address();
}

client::~client() {
    disconnect();
}

string client::address() const {
    struct sockaddr_storage addr;
    socklen_t len=sizeof addr;
    int a=getpeername(m_sock, (struct sockaddr*)&addr, &len);
    if (a!=0) return "";
    char ipstr[len]; // deal with both IPv4 and IPv6
    if (addr.ss_family == AF_INET) {
        struct sockaddr_in *s = (struct sockaddr_in *)&addr;
        int port = ntohs(s->sin_port);
        inet_ntop(AF_INET, &s->sin_addr, ipstr, len); //sizeof ipstr);
    } else { // AF_INET6
        struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
        int port = ntohs(s->sin6_port);
        inet_ntop(AF_INET6, &s->sin6_addr, ipstr, len); //sizeof ipstr);
    }
    return ipstr;
}

string client::connect(const string& host, uint16_t port, bool block) {
    lock_guard<mutex> lock(m_mx);
    assert(m_sock==0); //disconnect();
    string r=init_sock(host, port, block);
    if (!r.empty()) {
        return r;
    }
    m_addr=host;
    return "";
}

void client::disconnect() {
    lock_guard<mutex> lock(m_mx);
    if (unlikely(m_sock==0)) return;
    ::close(m_sock);
    m_sock=0;
}

void client::init_sockaddr (struct sockaddr_in *name, const char *hostname,	uint16_t port) {
    struct hostent *hostinfo;
    name->sin_family = AF_INET;
    name->sin_port = htons (port);
    hostinfo=::gethostbyname(hostname);
    if (hostinfo == 0) {
        cerr << "Unknown host " << hostname << endl;
        exit(EXIT_FAILURE);
    }
    name->sin_addr = *(struct in_addr *) hostinfo->h_addr;
}

string client::init_sock(const string& host, uint16_t port, bool block) {
    sockaddr_in servername;
    if (unlikely(m_sock!=0)) {
        m_sock=0;
        return "socket is non zero.";
    }

    m_sock=::socket(PF_INET, SOCK_STREAM, 0);// Create the socket.
    {
        auto flags = fcntl(m_sock, F_GETFL, 0);
        if (!block) flags|=O_NONBLOCK;
        fcntl(m_sock,F_SETFL,flags);
    }

    init_sockaddr (&servername, host.c_str(), port);//Connect to the server.
    int r=::connect(m_sock, (struct sockaddr *) &servername, sizeof (servername));
    if (unlikely(r<0)) {
        if (errno==EINPROGRESS) {
            typedef chrono::high_resolution_clock clock;
            using namespace chrono_literals;

            int error = 0;
            socklen_t len = sizeof (error);
            auto t1 = clock::now();
            struct sockaddr_storage addr;
            socklen_t lena=sizeof addr;

            while(true) {
                int a=getpeername(m_sock, (struct sockaddr*)&addr, &lena);
                if (a==0) break;
                if (chrono::duration_cast<std::chrono::milliseconds>(clock::now() - t1).count()>500) {
                    return "Error. Timeout obtaining peername";
                }
                this_thread::sleep_for(10ms);
            }
            return "";
        }

        ::close(m_sock);
        m_sock=0;
        ostringstream os;
        os << "Connection refused. errno=" << errno;
        return os.str();
    }

    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    setsockopt(m_sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
    return "";
}

pair<string,datagram*> client::send_recv(datagram* d,uint16_t expected_service) {
    pair<string,datagram*> ans;
    ans.first=send(d);
    if (unlikely(!ans.first.empty())) {
        ans.second=0;
        return move(ans);
    }
    return recv(expected_service);
}

pair<string,datagram*> client::send_recv(datagram* d) {
    pair<string,datagram*> ans;
    ans.first=send(d);
    if (unlikely(!ans.first.empty())) {
        ans.second=0;
        return move(ans);
    }
    return recv();
}

pair<string,datagram*> client::recv(uint16_t expected_service) { //caller owns the returning object
    auto r=recv();
    if (!r.first.empty()) {
        return move(r);
    }
    datagram*d=r.second;
    while(true) { ///delete garbage injected by hackers
        if (d->service==us::gov::protocol::gov_socket_error) {
            return move(r);
        }
        if (d->service==expected_service) {
            break;
        }
        else {
            delete d;
        }
        auto r=recv();
        if (!r.first.empty()) {
            return move(r);
        }
        d=r.second;
    }
    return make_pair("",d);
}

pair<string,datagram*> client::recv() { //caller owns the returning object
    pair<string,datagram*> r;
    r.second=new datagram();
    while(!program::_this.terminated) {
        string ans=r.second->recvfrom(m_sock);
        if (unlikely(!ans.empty())) {
            r.first=ans;
	        delete r.second;
            r.second=0;
            disconnect();
            break;
        }
        if (r.second->completed()) {
            break;
        }
    }
    return move(r);
}

string client::send(datagram* d) {
    if (unlikely(!m_sock)) {
        return "Error. Sending datagram before connecting.";
    }
    assert(d);
    auto r=d->sendto(m_sock);
    if (unlikely(!r.empty())) {
        disconnect();
    }

    delete d;
    return r;
}

string client::send(const datagram& d) {
    if (unlikely(!m_sock)) {
        return "Error. Sending datagram before connecting.";
    }

    auto r=d.sendto(m_sock);
    if (unlikely(!r.empty())) {
        disconnect();
    }
    return r;
}

void client::dump(ostream& os) const {
    os << "socket client: mem addr: " << this << " fd: " << m_sock
       << "; inet addr: " << m_addr << endl;
}

