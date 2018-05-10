#include "io.h"
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <cassert>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "datagram.h"
#include "server.h"
#include <gov/likely.h>

using namespace std;
using namespace us::gov::socket;

bool io::send(int sock, datagram*d) {
	auto b=d->send(sock);
	if (likely(b)) delete d;
	return b;
}

bool io::send(int sock, char d) {
        auto nbytes = server::os->write(sock, &d, 1);
        if (unlikely(nbytes<0)) {
                return false;
        }
        if (unlikely(nbytes!=1)) {
                return false;
        }
	return true;
}



