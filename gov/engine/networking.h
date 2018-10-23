#ifndef USGOV_75e4bd922f2de31a967ff5ad48fd3c21a0df7b241765114881ed8761ed8ca7cd
#define USGOV_75e4bd922f2de31a967ff5ad48fd3c21a0df7b241765114881ed8761ed8ca7cd

#include <us/gov/dfs/daemon.h>
#include <us/gov/peer/peer_t.h>
#include <us/gov/relay/peer_t.h>
#include <vector>
#include <unordered_set>
#include <string>
#include "us/gov/socket/client.h"

namespace us{ namespace gov{ namespace engine{
using namespace std;
struct daemon;

class networking: public dfs::daemon {
public:
    typedef dfs::daemon b;
    using b::keys;
 
    networking(engine::daemon* parent, const string& home);
    networking(uint16_t port, uint16_t edges, engine::daemon* parent, 
               const vector<string>& seed_nodes, const string& home);

public:
    virtual vector<relay::peer_t*> get_nodes() override;
    void dump(ostream& os) const;
    
    const engine::daemon* get_parent() const {
        return m_parent;
    }

    const vector<string>& get_seed_nodes() const {
        return m_seed_nodes;
    }

    void add_ip_to_seed_nodes(const string& ip){
        m_seed_nodes.push_back(ip);
    }

private:
    virtual bool process_work(socket::peer_t *c, socket::datagram*d) override;
    virtual bool process_evidence(socket::datagram*d) override;
    bool process_work_sysop(peer::peer_t *c, socket::datagram*d);
    //returns ipaddress //there exist a possibility of returning "" even though there were eligible items available
    virtual string get_random_peer(const unordered_set<string>& exclude) const override; 
    virtual dfs::peer_t* get_random_edge(const dfs::peer_t* exclude) const override;
    virtual dfs::peer_t* get_random_edge() const override;
    virtual const keys& get_keys() const override;
    virtual socket::client* create_client(int sock) override;
   
private:
    engine::daemon* m_parent;
    vector<string> m_seed_nodes;
};

}}}

#endif

