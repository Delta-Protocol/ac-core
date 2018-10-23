#ifndef USGOV_f0164e5b85e25bbdcb03ffe3d537b65893ccd1250bf2d05f3de97557fb69bb70
#define USGOV_f0164e5b85e25bbdcb03ffe3d537b65893ccd1250bf2d05f3de97557fb69bb70

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



