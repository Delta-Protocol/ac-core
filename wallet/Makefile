CPPS := $(wildcard *.cpp)
HS := $(wildcard *.h)

CXXFLAGS = -fPIC -g -O0 -DDEBUG -std=c++17

.PHONY: all
all: libuswallet.so

libuswallet.so: ${CPPS} ${HS}
	g++ ${CXXFLAGS} -shared ${CPPS} -pthread -lsecp256k1 -lusgov -L../gov -I../.. -o libuswallet.so

clean:
	rm -f libuswallet.so

