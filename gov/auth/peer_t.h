#ifndef USGOV_623b2ff2c7648665400db382bc9542b909646b0c7f35010ee4d34b7a4e819d4c
#define USGOV_623b2ff2c7648665400db382bc9542b909646b0c7f35010ee4d34b7a4e819d4c

#include <us/gov/id/peer_t.h>

namespace us{ namespace gov {
namespace auth {

using namespace std;

struct peer_t: id::peer_t {
       typedef id::peer_t b;
        enum stage_t {
            denied=0,
            authorized,
            num_stages
        };
        constexpr static array<const char*,num_stages> stagestr={"denied","authorized"};

        peer_t(int sock=0);
        virtual ~peer_t();

        virtual const keys& get_keys() const=0;

        virtual void verification_completed() override;

        virtual bool authorize(const pubkey_t& p) const=0;
        void dump(ostream& os) const;
        virtual void dump_all(ostream& os) const override {
                        dump(os);
                        b::dump_all(os);
                }

        //virtual string run_auth() override;

        stage_t stage{denied};
};

}
}}

#endif



