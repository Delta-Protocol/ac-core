#ifndef USGOV_f0164e5b85e25bbdcb03ffe3d537b65893ccd1250bf2d05f3de97557fb69bb70
#define USGOV_f0164e5b85e25bbdcb03ffe3d537b65893ccd1250bf2d05f3de97557fb69bb70

#include <us/gov/auth/peer_t.h>
#include "daemon.h"

namespace us { namespace wallet {
using namespace std;

struct peer_t: auth::peer_t {
   typedef auth::peer_t b;
   peer_t(int);
   virtual ~peer_t();

   virtual bool authorize(const pub_t& p) const override {
        return static_cast<wallet_daemon*>(parent)->authorize(p);
   }
   virtual const keys& get_keys() const;


    //TODO remove when java implements run_auth_responder
   virtual string run_auth_responder() override {
        stage_peer=verified;
        stage_me=verified;
        pubkey="25hWPEdydrHRapxuh4DjW5K5W4mHkNefew1zZ63dkGuoc";
        return "";
   }

};

}}

#endif



