#include "perf.h"
#include <cex/engine.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <vector>
#include <iostream>
#include <thread>
#include <atomic>

typedef ::perf c;

c::perf(counter& cnt): B("performance",cnt) {

}
using namespace std;
using namespace curex::cex;

mutex _outmx;

void create_account_task(engine& e, int thid) {
	curex::cex::ostream os(0);
	for (int i=0; i<100; ++i) {
		{
		lock_guard<mutex> lock(_outmx);		
		cout << "thid " << thid << ": " << i << endl;
		}
		e.new_account(L"888888888",os);
	}

}


#include <thread>
#include <assert.h>
void c::run() {
	B::run();


	engine e("cex_test_perf",wcerr);
	e.add_sample_liquidity(wcerr);

	vector<thread> v;
	for (int i=0; i<100; ++i) {
		v.emplace_back(create_account_task,ref(e),i);
	}
	for (int i=0; i<100; ++i) {
		v[i].join();
	}

	//multiples threads llamando a create_account
	


}	

