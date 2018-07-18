CPPS:=$(wildcard *.cpp)
HS:=$(wildcard *.h)
LIBS:=-lusgov -luswallet
FCGILIBS:=-lfastcgipp -ljsoncpp
CXXFLAGS:=-g -O0

.PHONY: all
all: us-wallet

us-wallet-fcgi: ${CPPS} ${HS}
	g++ -DFCGI ${CXXFLAGS} -pthread ${LIBS} ${FCGILIBS} -L../gov -L../wallet -I../.. ${CPPS} -o us-wallet

us-wallet: ${CPPS} ${HS}
	g++ ${CXXFLAGS} -pthread ${LIBS} -L../gov -L../wallet -I../.. ${CPPS} -o us-wallet

clean:
	rm -f us-wallet

