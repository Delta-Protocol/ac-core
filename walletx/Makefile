CPPS:=$(wildcard *.cpp)
HS:=$(wildcard *.h)
LIBS:=-lusgov -luswallet
FCGILIBS:=-lfastcgipp -ljsoncpp
CXXFLAGS = -g -O0 -std=c++17 -DDEBUG

.PHONY: all
all: us-wallet

us-wallet-fcgi: ${CPPS} ${HS}
	g++ -DFCGI ${CXXFLAGS} ${CPPS} -pthread ${LIBS} ${FCGILIBS} -L../gov -L../wallet -I../.. -o us-wallet

us-wallet: ${CPPS} ${HS}
	g++ ${CXXFLAGS} ${CPPS} -pthread ${LIBS} -L../gov -L../wallet -I../.. -o us-wallet

clean:
	rm -f us-wallet

