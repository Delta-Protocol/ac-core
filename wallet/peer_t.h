#ifndef US_GOV_WALLET_PEER_T_H
#define US_GOV_WALLET_PEER_T_H

#include <us/gov/auth/peer_t.h>
#include "daemon.h"

namespace us{ namespace wallet{
using namespace std;

class peer_t: public auth::peer_t {
public:
   peer_t(int);
   virtual ~peer_t();

   virtual bool authorize(const pub_t& p) const override {
        return static_cast<wallet_daemon*>(m_parent)->authorize(p);
   }
   virtual const keys& get_keys() const;
};
}}

#endif



