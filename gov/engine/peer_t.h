#ifndef US_GOV_ENGINE_PEER_T_H
#define US_GOV_ENGINE_PEER_T_H

#include <us/gov/dfs/daemon.h>

namespace us{ namespace gov{ namespace engine{
using namespace std;

class peer_t: public dfs::peer_t {
public:
    typedef dfs::peer_t b;
    enum stage_t {
        unknown=0,
        sysop,
        out,
        hall,
        node,
        wallet,
        num_stages
    };

    peer_t(int sock):b(sock) {}
    virtual ~peer_t() {
        disconnect();
    }

public:
    virtual void dump_all(ostream& os) const override {
        dump(os);
        b::dump_all(os);
    }
private:
    void dump(ostream& os) const;

private:
    virtual void verification_completed() override;
    virtual bool authorize(const pubkey_t& p) const override;
    virtual const keys& get_keys() const override;

public:
    void set_stage(const stage_t& s){
        m_stage = s; 
    }

    const stage_t& get_stage(){
        return m_stage;
    }

    constexpr static array<const char*,num_stages> m_stagestr={"unknown","sysop","out","hall","node","wallet"};
 
private:
    stage_t m_stage{unknown};
};

}}}

#endif

