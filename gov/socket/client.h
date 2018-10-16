#ifndef USGOV_dfd7f74406ecd7bf1a137eafe457ab52dcb9c50fe177017cc67b73de69834ecb
#define USGOV_dfd7f74406ecd7bf1a137eafe457ab52dcb9c50fe177017cc67b73de69834ecb

#include <atomic>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <string>
#include <utility>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "datagram.h"

namespace us { namespace gov { namespace socket {
using namespace std;

class client {

private:

    void init_sockaddr(struct sockaddr_in *name,
                       const char *hostname,
                       uint16_t port);

    string init_sock(const string& host,
                     uint16_t port,
                     bool block=false);

    mutable mutex m_mx;
    mutable string m_msg;

public:

    atomic<bool> m_busy{false};
    string m_addr;
    int m_sock;

    client();
    client(int sock);
    virtual ~client();

    inline bool connected() const { return m_sock!=0; }

    virtual string connect(const string& host,
                           uint16_t port,
                           bool block=false);

    virtual void disconnect();
    virtual void on_detach() {}

    void dump(ostream& os) const;
    virtual void dump_all(ostream& os) const { dump(os); }

    pair<string,datagram*> recv();
    pair<string,datagram*> recv(uint16_t expected_service);
    pair<string,datagram*> send_recv(datagram* d);
    pair<string,datagram*> send_recv(datagram* d,uint16_t expected_service);

    string send(const datagram& d);
    string send(datagram* d);
    string address() const;
};

}}}

#endif

