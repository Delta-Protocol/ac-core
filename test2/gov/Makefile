CPPS := $(wildcard *.cpp) $(wildcard */*.cpp)
HS := $(wildcard *.h) $(wildcard */*.h)
INC := ../../..
CXXFLAGS = -O0 -g -std=c++17
GOVUNITS := $(wildcard ./*/*.o) $(wildcard */*.o) $(wildcard .o)

export CXXFLAGS

.PHONY: all
all: clean ut_gov

ut_gov:
	g++ ${CXXFLAGS} -I${INC} ${HS} ${GOVUNITS} -pthread -lsecp256k1 -lcrypto++ -lusgov -lgtest -L/usr/lib -L../../../us/gov -c ${CPPS}

clean:
	find . -name "*.o" -delete
	find . -name "*.gch" -delete
	find . -name "*.class" -delete
	