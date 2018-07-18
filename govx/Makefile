#https://gist.github.com/xuhdev/1873316

CXXFLAGS = -g -O0 -std=c++17 -DDEBUG

.PHONY: all
all: us-gov

us-gov: main.cpp
	g++ ${CXXFLAGS} -pthread -I../.. main.cpp -lusgov -L../gov -L/usr/lib -o us-gov

clean:
	rm -f us-gov

