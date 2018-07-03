CPPS := $(wildcard *.cpp)
HS := $(wildcard *.h)
INC := ../..
CXXFLAGS = -O0 -g -std=c++17

.PHONY: all

all: unit_tests

unit_tests: ${HS} ${CPPS}
<<<<<<< HEAD
	g++ ${CXXFLAGS} -I${INC} ${CPPS} -lsecp256k1 -lcryptopp -lusgov -L/usr/local/lib -L../gov  -o unit_tests
=======
	g++ ${CXXFLAGS} -I${INC} -pthread -lusgov -L../gov ${CPPS} -o unit_tests
>>>>>>> origin

clean:
	rm -f unit_tests
	find . -name "*.o" -delete


