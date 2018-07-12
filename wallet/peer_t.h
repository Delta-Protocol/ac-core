#ifndef USGOV_c0147dc9f5504ae273f75dad4c55b65d9d068d20f69c77b18fff115fb67df993
#define USGOV_c0147dc9f5504ae273f75dad4c55b65d9d068d20f69c77b18fff115fb67df993

#include <us/gov/auth.h>

namespace us{ namespace wallet {
using namespace std;

struct peer_t: gov::auth::peer_t {
       typedef gov::auth::peer_t b;
        enum stage_t {
            denied=0,
            authorized,
            num_stages
        };
        constexpr static array<const char*,num_stages> stagestr={"denied","authorized"};

        virtual const keys& get_keys() const override;

        peer_t(const keys&, int sock);
        virtual ~peer_t();
 
        keys k;
};

}}

#endif



