#ifndef CUREX_TEST_API_
#define CUREX_TEST_API_

#include <jail/unit_test.h>


	class api: public ::test::unit_test {
		typedef ::test::unit_test B;
	public:
		api(counter&);
		void run();


	};


#endif
