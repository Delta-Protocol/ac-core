#include <iostream>
#include "split_function.h"		//	 <us/test/gov/dfs/split_function.h>

#include <us/gov/socket/datagram.h>


using namespace std;

int main() {


//		g++ -std=c++17 -I../../../.. -L../../../gov -lusgov main.cpp split_function.cpp
//		LD_LIBRARY_PATH=../gov ./a.out


	testing_split_function();


	exit(0);
}
