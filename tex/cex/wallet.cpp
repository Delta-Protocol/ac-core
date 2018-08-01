#include "wallet.h"

using namespace curex::cex;
using namespace std;
typedef curex::cex::wallet c;

c::wallet() {
}
c::wallet(const efp& av): available(av) {
}

void c::deposit(const efp& amount) {
	available+=amount;
}
efp c::withdraw(const efp& amount) { //returns amount withdrawn
	efp am;
	if (amount>available) {
		am=available;
	}
	else {
		am=amount;
	}
	available-=am;
	return am;
}


