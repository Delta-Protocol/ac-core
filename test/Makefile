CPPS := $(wildcard *.cpp)
HS := $(wildcard *.h)
INC := ../..
CXXFLAGS = -O0 -ggdb -std=c++17

.PHONY: all

all: unit_tests

unit_tests: ${HS} ${CPPS}
	g++ ${CXXFLAGS} -I${INC} ${CPPS} -lsecp256k1 -lcryptopp -lusgov -L/usr/local/lib -L../gov  -o unit_tests

clean:
	rm -f unit_tests
	find . -name "*.o" -delete


