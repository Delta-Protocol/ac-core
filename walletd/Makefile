CPPS := $(wildcard *.cpp)
HS := $(wildcard *.h)

CXXFLAGS:=-g -O0

.PHONY: all
all: us-walletd

us-walletd: ${CPPS} ${HS}
	g++ ${CXXFLAGS} -pthread -lusgov -luswallet -lfastcgipp -L../gov -L../wallet -I../.. ${CPPS} -o us-walletd

clean:
	rm -f us-walletd

