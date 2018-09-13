CPPS := $(wildcard *.cpp)
HS := $(wildcard *.h)
INC := ../../../..
CXXFLAGS = -O0 -g -std=c++17

export CXXFLAGS

.PHONY: all
all: clean unit_tests

unit_tests: ${HS} ${CPPS}
	g++ ${CXXFLAGS} -I${INC} ${CPPS} -pthread -lsecp256k1 -lcrypto++ -lusgov -L/usr/lib -L../../../../us/gov -o dfs_unit_tests.o

clean:
	find . -name "*.o" -delete
	find . -name "*.class" -delete
	