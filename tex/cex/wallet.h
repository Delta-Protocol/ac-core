#ifndef CUREX_CEX_WALLET_H
#define CUREX_CEX_WALLET_H
#include "cex.h"

namespace curex { namespace cex {

class wallet {
public:
	wallet();
	wallet(const efp& available);
	void deposit(const efp& amount);
	efp withdraw(const efp& amount);
	
	efp available{0};

};


}}

#endif

