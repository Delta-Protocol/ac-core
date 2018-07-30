#ifndef CUREX_CEX_EFP_H
#define CUREX_CEX_EFP_H

#include "cex.h"
#include <utility>


namespace curex { namespace cex {

	typedef long long efp;
	static const efp precision=5; //4
	static const efp one=100000; //10000
	static const efp rate_multiples=5; //50

	static const efp invalid_efp=0;

	double decode_dbl(const efp& v);
	string decode(const efp& v);
	std::string decode_std(const efp& v);
	efp encode(const double& v);
	efp encode(string s);
	std::pair<efp,efp> encode(const double& net,const double& comm, const efp& sum);




}}
#endif

