CPPS := $(wildcard *.cpp)
HS := $(wildcard *.h)

CXXFLAGS := -fPIC ${CXXFLAGS}

.PHONY: all
all: libuswallet.so

libuswallet.so: ${CPPS} ${HS}
	g++ ${CXXFLAGS} -shared -pthread -lsecp256k1 -lusgov -L../gov -I.. ${CPPS} -o libuswallet.so

clean:
	rm -f libuswallet.so

