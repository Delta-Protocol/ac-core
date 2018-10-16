#ifndef USGOV_c240798d9f2ba9cd92a0728e617b7d39e33fe0519df7744799c27d2312bc782b
#define USGOV_c240798d9f2ba9cd92a0728e617b7d39e33fe0519df7744799c27d2312bc782b

#include <atomic>
#include <mutex>
#include <string>
#include <vector>
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

#include "us/gov//crypto/ec.h"
#include "us/gov/signal_handler.h"
#include "client.h"

namespace us { namespace gov { namespace socket {

class datagram;

class server: public signal_handler::callback {

private:

    uint16_t m_port;
    int m_sock {0};

    bool banned_throttle(const string& addr);

    struct clients_t: unordered_map<int,client*> {
        typedef unordered_map<int,client*> b;

        clients_t() {}
        clients_t(const clients_t& other)=delete;
        ~clients_t();

        struct rmlist:unordered_set<client*> {
            typedef unordered_set<client*> b;
            ~rmlist();
            bool add(client* c);
            void dump(ostream& os) const;
            mutable mutex mx;
        };

        struct wait:unordered_set<client*> {
            typedef unordered_set<client*> b;
            ~wait();
            void add(client* c);
            bool remove(client* c);
            bool find(const client& c) const;
            void dump(ostream& os) const;
            mutable mutex mx;
        };

        struct attic_t:unordered_set<client*> {
            ~attic_t();
            void add(client*);
            void purge();
        };

        void add(client* c, bool wakeupselect=true);
        void remove(client* c);
        void grow();
        void shrink();
        vector<int> update();

        vector<client*> active() const {
            lock_guard<mutex> lock(mx_active);
            return active_;
        }

        mutable mutex mx_active;
        vector<client*> active_;

        void dump(ostream& os) const;
        void read_sockets();
        client locli; //loopback;

    private:
        mutable mutex mx;
        wait wadd;
        rmlist wremove;
        attic_t attic;
    };

protected:

    virtual void receive_and_process(client*c) { c->m_busy.store(false); };
    virtual void attach(client*,bool wakeupselect=true);
    virtual client* create_client(int sock);
    virtual void on_finish() override;

    clients_t m_clients;
    vector<client*> active() const { return m_clients.active(); }

public:

    server();
    server(uint16_t port);
    virtual ~server();

    datagram* read_from_client(int sock);
    int make_socket (uint16_t port);
    void dump(ostream& os) const;
    void run();

    virtual void detach(client*);

    typedef crypto::ec::keys::pub_t pub_t;
};

}}}

#endif

