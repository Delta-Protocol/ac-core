CPPS:=$(wildcard *.cpp)
HS:=$(wildcard *.h)
LIBS:=-lusgov -luswallet -ljsoncpp
FCGILIBS:=-lfastcgipp 
CXXFLAGS = -g -O0 -std=c++17 -DDEBUG

.PHONY: all
all: us-wallet

us-wallet: ${CPPS} ${HS}
ifeq ($(FCGI),1)
	g++ -DFCGI ${CXXFLAGS} ${CPPS} -pthread ${LIBS} ${FCGILIBS} -L../gov -L../wallet -I../.. -o us-wallet
else
	g++ ${CXXFLAGS} ${CPPS} -pthread ${LIBS} -L../gov -L../wallet -I../.. -o us-wallet
endif

clean:
	rm -f us-wallet

