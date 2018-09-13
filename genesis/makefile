#https://gist.github.com/xuhdev/1873316
CXXFLAGS:=-O3 -std=c++17

.PHONY: all
all: genesis

genesis: main.cpp
	g++ ${CXXFLAGS} -pthread -I../.. main.cpp -lusgov -L../gov -L/usr/lib -o genesis

clean:
	rm -f genesis

