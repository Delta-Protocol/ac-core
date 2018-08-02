#include <jail/unit_test.h>
#include "front.h"
#include "api.h"
#include "perf.h"

int main(int argc, char **argv) {
	test::unit_test::counter c;
	{front o(c); o.run(); }
//	{api o(c); o.run(); }
	//{perf o(c); o.run(); }
	return 0;
}

