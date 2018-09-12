#https://gist.github.com/xuhdev/1873316
CPPS:=$(wildcard *.cpp)
HS:=$(wildcard *.h)
CXXFLAGS = -g -O0 -std=c++17 -DDEBUG

.PHONY: all
all: us-gov

us-gov: ${CPPS} ${HS}
	g++ ${CXXFLAGS} ${CPPS} -pthread -I../.. -lusgov -L../gov -L/usr/lib -o us-gov

clean:
	rm -f us-gov

