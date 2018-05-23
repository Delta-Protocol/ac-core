#ifndef USGOV_5369cd4184b5cf59017909fa4ac73e6f59eb5af656bfd6f5212953d549e6aba5
#define USGOV_5369cd4184b5cf59017909fa4ac73e6f59eb5af656bfd6f5212953d549e6aba5

#include <us/gov/dfs/daemon.h>

namespace us { namespace gov {
namespace blockchain {
	using namespace std;

	struct peer_t:dfs::peer_t {
		typedef dfs::peer_t b;
		enum stage_t {
			unknown=0,
			sysop,
			out,
			hall,
			node,
			num_stages
		};
		constexpr static array<const char*,num_stages> stagestr={"unknown","sysop","out","hall","node"};

		peer_t(int sock):b(sock) {}
		virtual ~peer_t() {}

		void dump(ostream& os) const;

		virtual void verification_completed() override;
/*
public:
        stage_t get_stage() const { return stage; }
        void set_stage(stage_t s) { stage=s; }
private:
*/
		stage_t stage{unknown};
	};


}
}}

#endif

