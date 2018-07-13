#include "us/gov/crypto/symmetric_encryption.h"

#include <iostream>
#include <string>
#include "us/gov/crypto/ec.h"

using namespace std;
using namespace us::gov::crypto;

int main ()
{
    ec::keys k = ec::keys::generate();
    std::cout << k.priv.to_b58() << " " << k.pub.to_b58() << endl;
}