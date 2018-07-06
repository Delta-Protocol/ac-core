#https://gist.github.com/xuhdev/1873316

.PHONY: all
all: us-govd

us-govd: main.cpp
	g++ ${CXXFLAGS} -pthread -I../.. main.cpp -lusgov -L../gov -L/usr/lib -o us-govd

clean:
	rm -f us-govd

