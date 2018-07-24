#include "us/gov/crypto/symmetric_encryption.h"

#include <iostream>
#include <string>
#include "us/gov/crypto/ec.h"

using namespace std;
using namespace us::gov::crypto;

int main ()
{
    ec::keys k = ec::keys::generate();
    //std::cout << k.priv << " " << k.pub << endl;
    //std::cout << "4ACKBcXXhtGb3NtZzSfwgSs3GqCgCBY65juF2UVSJQR2 dPtUg631Hh7tL8t3wK6KHDwFfQzLmcHtkEAH5mSsANX3" << endl;
    std::cout << "7RZ9FmWLCdfGZCzThU7DZKf2q8hyxtAzCXfjXEGVU8E1 R9s12KNiCKJG1ax5hYGoSHgLSMX3trHY26jmwbhUQFRQCgwR2gRtrAnJTR8NwmFkD4YMtHtXhe57KAd6nSjrYtMV" << endl;
}