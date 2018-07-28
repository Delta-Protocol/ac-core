#ifndef USGOV_f195771f72f8dc2390e21a5c2a022838551620e7e997e90438f201a2aa3aa216
#define USGOV_f195771f72f8dc2390e21a5c2a022838551620e7e997e90438f201a2aa3aa216

#include <us/api/apitool_generated_wallet.h>
#include <string>
#include <jsoncpp/json/json.h> 

namespace us { namespace wallet {
namespace json {
using namespace std;

struct wallet_api:us::api::wallet {
	wallet_api(us::api::wallet* underlying_api);
	virtual ~wallet_api();

#include <us/api/apitool_generated_wallet_functions_cpp_override>

    us::api::wallet* underlying_api;
};

}
}}

#endif



