CPPS := $(wildcard *.cpp) $(wildcard */*.cpp)
HS := $(wildcard *.h) $(wildcard */*.h)
CXXFLAGS = -O0 -g -std=c++17

.PHONY: all
all: clean ut_wallet

ut_wallet:
	g++ ${CXXFLAGS} -I./ -I../ -I../../ -I../../../ ${HS} -pthread -lcrypto++ -lgtest -L../../gov -L../../wallet -L/usr/lib -lusgov -c ${CPPS}

clean:
	find . -name "*.o" -delete
	find . -name "*.gch" -delete
	find . -name "*.class" -delete
