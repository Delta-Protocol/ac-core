#https://gist.github.com/xuhdev/1873316

.PHONY: all
all: us-govd

us-govd: main.cpp
	g++ ${CXXFLAGS} -pthread -lusgov -L../gov -I../.. main.cpp -o us-govd

clean:
	rm -f us-govd

