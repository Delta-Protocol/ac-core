CPPS := $(wildcard *.cpp)
HS := $(wildcard *.h)

CXXFLAGS=-g -O0

.PHONY: all
all: us-wallet

us-wallet: ${CPPS} ${HS}
	g++ ${CXXFLAGS} -pthread -lusgov -L../gov -I.. ${CPPS} -o us-wallet

clean:
	rm -f us-wallet

