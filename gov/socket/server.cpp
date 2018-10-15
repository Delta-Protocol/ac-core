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

using namespace std;
using namespace us::gov::socket;

server::server():port(0) {
}

server::server(uint16_t p):port(p) {
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

    { int optval = 1; setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval); }

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
    clients.read_sockets();
}

void server::attach(client*c, bool wakeupselect) {
    assert(c);
    assert(c->m_sock!=0);
    clients.add(c,wakeupselect);
}

void server::detach(client*c) {
    assert(c);
    clients.remove(c);
}

void server::clients_t::add(client* c, bool wakeupselect) {
    wadd.add(c);
    if (wakeupselect) read_sockets();
}

void server::clients_t::remove(client* c) {
    if (wremove.add(c))
        read_sockets();
}

void server::clients_t::read_sockets() {
    static char w='w';
    ::write(locli.m_sock, &w, 1);
}

void server::clients_t::grow() {
    unordered_set<client*> t;
    {
        lock_guard<mutex> lock(wadd.mx);
        t=wadd;
        wadd.clear();
    }
    {
        lock_guard<mutex> lock(mx);
        for (auto i:t) {
            assert(i);
            assert(find(i->m_sock)==end());
            assert(i->m_sock!=0);
            emplace(i->m_sock,i);
        }
    }
    if (!t.empty()) read_sockets();
}

void server::clients_t::shrink() {
    unordered_set<client*> copy;
    {
        lock_guard<mutex> lock(wremove.mx);
        copy=wremove;
        wremove.clear();
    }

    lock_guard<mutex> lock(mx);
    for (auto c:copy) {
        iterator i;
        c->on_detach();
        i=find(c->m_sock);
        if (likely(i!=end())) { //it is not in main container
            erase(i); //no other thread can remove from main container
            attic.add(c);
        }
        else {
            if (wadd.remove(c)) {
                attic.add(c);
            }
        }
    }
}

server::clients_t::~clients_t() {
    {
        lock_guard<mutex> lock(mx);
        for (auto i:*this) wremove.add(i.second); // flush me
    }

    shrink(); // flush wremove
}

vector<int> server::clients_t::update() {
    shrink();
    grow();
    attic.purge();
 
    vector<int> s;
    vector<client*> a;

    {
        lock_guard<mutex> lock(mx);
        s.reserve(size());
        a.reserve(size());
        for (auto i=begin(); i!=end();) {
            auto& c=*i->second;
            if (likely(c.m_sock) ) { //disconnected client?
                if (!c.m_busy.load()) s.emplace_back(c.m_sock);
                    a.emplace_back(&c);
                ++i;
            }
            else {
                c.on_detach();
                attic.add(i->second);
                i=erase(i);
            }
        }
    }

    {
        lock_guard<mutex> lock(mx_active);
        active_=a;
    }

    return move(s);
}

server::clients_t::attic_t::~attic_t() {
    for (auto&i:*this) delete i;
}

void server::clients_t::attic_t::add(client*cl) {
    emplace(cl);
}

void server::clients_t::attic_t::purge() {
    for (auto i=begin(); i!=end(); ) {
        if ((*i)->m_busy.load()) {
            ++i;
            continue;
        }
        else {
            delete *i;
            i=erase(i);
           
        }
    }
}

void server::dump(ostream& os) const {
    os << "Hello from socker::server" << endl;
    os << "Listening socket: " << sock << endl;
    os << "Clients: " << endl;

    clients.dump(os);
}

void server::clients_t::dump(ostream& os) const {
    int n=0;
    for (auto& i:active()) {
        os << "client #" << n++ << ": "; i->dump(os); os << endl;
    }

    os << "Total active: " << n << endl;
    os << "*add buffer: " << endl;
    wadd.dump(os);

    os << "*remove buffer: " << endl;
    wremove.dump(os);
}

server::clients_t::rmlist::~rmlist() {
    lock_guard<mutex> lock(mx);
    assert(empty());
}

bool server::clients_t::rmlist::add(client* c) {
    if(c->m_sock==0) return false;
    lock_guard<mutex> lock(mx);
    emplace(c);
    return true;
}

void server::clients_t::rmlist::dump(ostream& os) const {
    lock_guard<mutex> lock(mx);
    os << "Size: " << size() << endl;
    for (auto i:*this) {
        i->dump_all(os);
        os << endl;
    }
}

server::clients_t::wait::~wait() {
    lock_guard<mutex> lock(mx);
    for (auto i:*this) delete i;
}

void server::clients_t::wait::add(client* c) {
    lock_guard<mutex> lock(mx);
    emplace(c);
}

bool server::clients_t::wait::remove(client* c) { //dont delete
    lock_guard<mutex> lock(mx);
    iterator i=b::find(c);
    if (i==end()) return false;
    erase(i);
    return true;
}

bool server::clients_t::wait::find(const client& c) const {
    lock_guard<mutex> lock(mx);
    return b::find(const_cast<client*>(&c))!=end();
}

void server::clients_t::wait::dump(ostream& os) const {
    lock_guard<mutex> lock(mx);
    os << "Size: " << size() << endl;
    for (auto i:*this) {
        i->dump_all(os);
        os << endl;
    }
}

client* server::create_client(int sock) {
    return new client(sock);
}

void server::run() {
    fd_set read_fd_set;
    int i;
    struct sockaddr_in clientname;
    unsigned int size;
    assert(port);
    sock = make_socket(port);

    if (!sock) {
        cerr << "error making socket" << endl;
        program::_this.finish();
        return;
    }

    if (::listen (sock, 1) < 0) {
        cerr << "error listen" << endl;
        close(sock);
        sock=0;
        program::_this.finish();
        return;
    }

    auto r=clients.locli.connect("127.0.0.1",port);
    if (unlikely(!r.empty())) {
        close(sock);
        sock=0;
        cerr << "failed connecting the loopback client. " << r << endl;
        program::_this.finish();
        return;
    }

    // Initialize the set of active sockets.
    int loopback;
    size = sizeof (clientname);
    loopback = ::accept(sock, (struct sockaddr *) &clientname,&size);
    if (loopback < 0) {
        cerr << "error in ::accept" << endl;
        close(sock);
        program::_this.finish();
        return;
    }

    signal_handler::_this.add(this);
    char discard[4]; //loopback recv buffer, up to 30 wake up signals

    while (true) {
        FD_ZERO(&read_fd_set);
        FD_SET(sock,&read_fd_set);
        FD_SET(loopback,&read_fd_set);
        vector<int> sl=clients.update();

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

        if (FD_ISSET(sock, &read_fd_set)) {
            int nnew;
            size = sizeof (clientname);
            nnew = ::accept(sock, (struct sockaddr *) &clientname,&size);

            if (nnew < 0) {
                cerr << "error in ::accept 2" << endl;
                continue;
            }

            auto cl=create_client(nnew);
            if (unlikely(banned_throttle(cl->m_addr))) {
                delete cl;
            }
            else {
                attach(cl,false);
            }
        }

        for (int i:sl) { //Service all the sockets with input pending.
            if (likely(!FD_ISSET (i, &read_fd_set))) continue;

            //no need lock, this thread is the only that changes size of clients
            auto c=clients.find(i);
            if (unlikely(c==clients.end())) {
                cerr << "data arrived for an unknown fd " << i << endl;
                continue;
            }
            auto p=c->second;

            if (p->m_busy.load()) continue;
            p->m_busy.store(true);
            receive_and_process(p);
        }
    }

    signal_handler::_this.remove(this);
    close(sock);
    close(loopback);
    sock=0;
    loopback=0;
    clients.locli.disconnect();
}

