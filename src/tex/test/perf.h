#ifndef CUREX_TEST_PERF_
#define CUREX_TEST_PERF_

#include <jail/unit_test.h>


	class perf: public ::test::unit_test {
		typedef ::test::unit_test B;
	public:
		perf(counter&);
		void run();


	};


#endif
