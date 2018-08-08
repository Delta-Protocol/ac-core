#include "us/gov/crypto/symmetric_encryption.h"

#include <iostream>
#include <string>
#include "us/gov/crypto/ec.h"

using namespace std;
using namespace us::gov::crypto;

int main ()
{
    ec::keys k = ec::keys::generate();
    std::cout << k.priv << " " << k.pub << endl;
    //std::cout << "8L7rYfQYDx39du8JzcaNzZGZFfEPc46JdgE17kt65KbF 21bAuDz5A48N1qXDDvzvnT3BBrxfaa6uErMyByJ75gy4B" <<endl;
}
