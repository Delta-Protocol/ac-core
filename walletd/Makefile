CPPS:=$(wildcard *.cpp)
HS:=$(wildcard *.h)
LIBS:=-lusgov -luswallet
FCGILIBS:=-lfastcgipp -ljsoncpp
CXXFLAGS:=-g -O0

.PHONY: all
all: us-walletd

us-walletd-fcgi: ${CPPS} ${HS}
	g++ -DFCGI ${CXXFLAGS} -pthread ${LIBS} ${FCGILIBS} -L../gov -L../wallet -I../.. ${CPPS} -o us-walletd

us-walletd: ${CPPS} ${HS}
<<<<<<< HEAD
	g++ ${CXXFLAGS} -pthread ${CPPS} -lusgov -luswallet -lfastcgipp -ljsoncpp -L../gov -L../wallet -I../.. -o us-walletd
=======
	g++ ${CXXFLAGS} -pthread ${LIBS} -L../gov -L../wallet -I../.. ${CPPS} -o us-walletd
>>>>>>> origin

clean:
	rm -f us-walletd

