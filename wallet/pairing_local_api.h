
#ifndef USGOV_501a1656c74ab5296d94d111b535c5b05fb98098d2116c600b391f6e612eca22
#define USGOV_501a1656c74ab5296d94d111b535c5b05fb98098d2116c600b391f6e612eca22

#include "pairing.h"

namespace us{ namespace wallet {
using namespace std;

    struct pairing_local_api: pairing::api, pairing {
        typedef pairing b;
        pairing_local_api(const string& homedir);
        virtual ~pairing_local_api();
        virtual void pair(const pub_t&, const string& name, ostream&os) override;
        virtual void unpair(const pub_t&, ostream&os) override;
        virtual void list_devices(ostream&os) override;
    };


}}

#endif



