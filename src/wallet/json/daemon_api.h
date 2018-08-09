#ifndef USGOV_f195771f72f8dc2390e21a5c2a022838551620e7e997e90438f201a2aa3aa216
#define USGOV_f195771f72f8dc2390e21a5c2a022838551620e7e997e90438f201a2aa3aa216

#include <us/wallet/daemon_api.h>
#include <string>
#include <jsoncpp/json/json.h> 

namespace us { namespace wallet {
namespace json {
using namespace std;

struct daemon_api: us::wallet::daemon_api {
	daemon_api(us::wallet::daemon_api*);
	virtual ~daemon_api();

#include <us/api/apitool_generated__functions_wallet_cpp_override>
#include <us/api/apitool_generated__functions_pairing_cpp_override>

    us::wallet::daemon_api* underlying_api;
};

}
}}

#endif



