CPPS := $(wildcard *.cpp)
HS := $(wildcard *.h)
INC := ../..
CXXFLAGS = -O0 -g -std=c++17

all: buildWalletTest

buildWalletTest:
	g++ ${CXXFLAGS} -I${INC} ${CPPS} -pthread -lcrypto++ -I../../.. -L../../gov -L/usr/lib -lusgov -o walletTest.o

clean:
	find . -name "*.o" -delete
	find . -name "*.class" -delete

.PHONY: runTest
runTest:
	echo 'hello'
