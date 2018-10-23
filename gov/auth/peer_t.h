#ifndef USGOV_623b2ff2c7648665400db382bc9542b909646b0c7f35010ee4d34b7a4e819d4c
#define USGOV_623b2ff2c7648665400db382bc9542b909646b0c7f35010ee4d34b7a4e819d4c

#include <us/gov/id/peer_t.h>

namespace us{ namespace gov{ namespace auth{

using namespace std;

class peer_t: public id::peer_t {
public:    
    peer_t(int sock=0):id::peer_t(sock) {}
    virtual ~peer_t(){}

    enum stage_t {
        denied=0,
        authorized,
        num_stages
    };

    void set_stage(stage_t s){
        m_stage = s;
    }

    stage_t get_stage() const {
        return m_stage;
    }
    
protected:
    virtual const keys& get_keys() const=0;
    virtual void verification_completed() override;
    virtual bool authorize(const pubkey_t& p) const=0;
    virtual void dump_all(ostream& os) const override {
                                       dump(os);
                                       id::peer_t::dump_all(os);
    }

private:
    void dump(ostream& os) const;
  
private:
    constexpr static array<const char*,num_stages> m_stagestr={"denied","authorized"};
    stage_t m_stage{denied};
};

}}}

#endif



