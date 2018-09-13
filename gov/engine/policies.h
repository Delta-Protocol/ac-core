#ifndef USGOV_6c9e30b55f8a9529f6284516edcde739a1fb239a4ec9403f8f7614eb8da58f0d
#define USGOV_6c9e30b55f8a9529f6284516edcde739a1fb239a4ec9403f8f7614eb8da58f0d

#include <string>
#include <array>

namespace us { namespace gov {
namespace engine {
	using namespace std;
	struct param_t {

		enum consensus_t { majority=0, average, median, num_types };
		param_t(int id, consensus_t type): id(id), consensus_type(type) {
		}
		constexpr static array<const char*,num_types> typestr={"majority","average","median"};

		inline bool operator <(const param_t& other) const { return id < other.id; }
		int id;
		consensus_t consensus_type;
	};
}}
}
#endif

