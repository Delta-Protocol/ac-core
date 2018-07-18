#ifndef USGOV_a77bb134f65a2d28d9cf7823a14041036d606c371217ddd8b91b59f77a73b6b1
#define USGOV_a77bb134f65a2d28d9cf7823a14041036d606c371217ddd8b91b59f77a73b6b1

#include <cstdint>
#include <string>
#include <iostream>


namespace us { namespace gov {
namespace blockchain {
	using namespace std;
	struct daemon;
	struct shell {
		shell(daemon&d):d(d) {}
		void help(ostream&) const;
		string command(const string& cmd);

		daemon& d;
		int level{5};
		int cur_app{-1};
		string memory_cmd_lvl; //commands compatible across level, repeatable after level changes
	};


}}
}

#endif

