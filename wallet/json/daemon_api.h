#ifndef USGOV_f195771f72f8dc2390e21a5c2a022838551620e7e997e90438f201a2aa3aa216
#define USGOV_f195771f72f8dc2390e21a5c2a022838551620e7e997e90438f201a2aa3aa216

#include <us/api/apitool_generated_wallet_daemon.h>
#include <string>
#include <jsoncpp/json/json.h> 

namespace us { namespace wallet {
namespace json {
using namespace std;

struct daemon_api:us::api::wallet_daemon {
	daemon_api(us::api::wallet_daemon*);
	virtual ~daemon_api();

#include <us/api/apitool_generated_wallet_functions_cpp_override>
#include <us/api/apitool_generated_pairing_functions_cpp_override>

    us::api::wallet_daemon* underlying_api;
};

}
}}

#endif



