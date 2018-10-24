/****************************************************************************
 * Copyright (C) 2018 by                                                    *
 ****************************************************************************/

#include "server.h"

#include <asm-generic/socket.h>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>
#include <algorithm>
#include <cassert>
#include <utility>

#include "us/gov/likely.h"

using namespace us::gov::socket;

server::server():m_port(0) {
}

server::server(uint16_t p):m_port(p) {
}

server::~server() {
}

int server::make_socket (uint16_t port) {
    int sock;
    struct sockaddr_in name;
    sock=::socket(PF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "socket: server: could not create socket" << endl;
        return 0;
    }

    {
        int optval = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
    }

    name.sin_family = AF_INET;
    name.sin_port = htons (port);
    name.sin_addr.s_addr = htonl (INADDR_ANY);
    if (::bind(sock, (struct sockaddr *) &name, sizeof (name)) < 0) {
        cerr << "port " << port << " is busy." << endl;
        return 0;
    }
    return sock;
}

bool server::banned_throttle(const string& addr) {
return false;
    static unordered_set<string> throttle;
    auto x=throttle.find(addr);
    if (x==throttle.end()) {
        throttle.emplace(addr);
        return false;
    }
    else {
        return true;
    }
}

void server::on_finish() {
    m_clients.read_sockets();
}

void server::attach(client*c, bool wakeupselect) {
    assert(c);
    assert(c->get_sock()!=0);
    m_clients.add(c,wakeupselect);
}

void server::detach(client*c) {
    assert(c);
    m_clients.remove(c);
}

void server::dump(ostream& os) const {
    os << "Hello from socker::server" << endl;
    os << "Listening socket: " << m_sock << endl;
    os << "Clients: " << endl;

    m_clients.dump(os);
}

client* server::create_client(int sock) {
    return new client(sock);
}

void server::run() {
    fd_set read_fd_set;
    int i;
    struct sockaddr_in clientname;
    unsigned int size;
    assert(m_port);
    m_sock = make_socket(m_port);

    if (!m_sock) {
        cerr << "error making socket" << endl;
        program::_this.finish();
        return;
    }

    if (::listen (m_sock, 1) < 0) {
        cerr << "error listen" << endl;
        close(m_sock);
        m_sock=0;
        program::_this.finish();
        return;
    }

    auto r=m_clients.connect("127.0.0.1",m_port);
    if (unlikely(!r.empty())) {
        close(m_sock);
        m_sock=0;
        cerr << "failed connecting the loopback client. " << r << endl;
        program::_this.finish();
        return;
    }

    // Initialize the set of active sockets.
    int loopback;
    size = sizeof (clientname);
    loopback = ::accept(m_sock, (struct sockaddr *) &clientname,&size);
    if (loopback < 0) {
        cerr << "error in ::accept" << endl;
        close(m_sock);
        program::_this.finish();
        return;
    }

    signal_handler::_this.add(this);
    char discard[4]; //loopback recv buffer, up to 30 wake up signals

    while (true) {
        FD_ZERO(&read_fd_set);
        FD_SET(m_sock,&read_fd_set);
        FD_SET(loopback,&read_fd_set);
        vector<int> sl=m_clients.update();

        for (auto& i:sl) {
            assert(i!=0);
            FD_SET(i,&read_fd_set); 
        }

        if (unlikely(::select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) < 0)) {
            cerr << "error in select" << endl;
            continue;
        }

        if (unlikely(thread_::_this.terminated)) break;
        if (unlikely(FD_ISSET(loopback, &read_fd_set))) {
            ::recv(loopback,&discard,4,0);
        }

        if (FD_ISSET(m_sock, &read_fd_set)) {
            int nnew;
            size = sizeof (clientname);
            nnew = ::accept(m_sock, (struct sockaddr *) &clientname,&size);

            if (nnew < 0) {
                cerr << "error in ::accept 2" << endl;
                continue;
            }

            auto cl=create_client(nnew);
            if (unlikely(banned_throttle(cl->get_address()))) {
                delete cl;
            }
            else {
                attach(cl,false);
            }
        }

        for (int i:sl) { //Service all the sockets with input pending.
            if (likely(!FD_ISSET (i, &read_fd_set))) continue;

            //no need lock, this thread is the only that changes size of clients
            auto c=m_clients.find(i);
            if (unlikely(c==m_clients.end())) {
                cerr << "data arrived for an unknown fd " << i << endl;
                continue;
            }
            auto p=c->second;

            if (p->load_busy()) 
                continue;
            p->store_busy(true);
            receive_and_process(p);
        }
    }

    signal_handler::_this.remove(this);
    close(m_sock);
    close(loopback);
    m_sock=0;
    loopback=0;
    m_clients.disconnect();
}

