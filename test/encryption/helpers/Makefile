CPPS := $(wildcard *.cpp)
HS := $(wildcard *.h)
INC := ../..
CXXFLAGS = -O0 -g -std=c++17

all: generate_keys_cpp

generate_keys_cpp:
	g++ ${CXXFLAGS} -I${INC} ${CPPS} -pthread -lcrypto++ -I../../.. -L../../gov -lusgov -L/usr/lib -o ./generate_keys_cpp.o

clean:
	find . -name "*.o" -delete
	find . -name "*.class" -delete
