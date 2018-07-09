CPPS:=$(wildcard *.cpp)
HS:=$(wildcard *.h)
LIBS:=-lusgov -luswallet
FCGILIBS:=-lfastcgipp -ljsoncpp
CXXFLAGS:=-g -O0

.PHONY: all
all: us-wallet

us-wallet-fcgi: ${CPPS} ${HS}
	g++ -DFCGI ${CXXFLAGS} -pthread ${CPPS} ${LIBS} ${FCGILIBS} -L../gov -L../wallet -I../.. -o us-wallet

us-wallet: ${CPPS} ${HS}
	g++ ${CXXFLAGS} -pthread ${CPPS} ${LIBS} -L../gov -L../wallet -I../.. -o us-wallet

clean:
	rm -f us-wallet

