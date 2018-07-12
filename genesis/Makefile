#https://gist.github.com/xuhdev/1873316

.PHONY: all
all: genesis

genesis: main.cpp
	g++ ${CXXFLAGS} -pthread -I../.. main.cpp args.cpp -lusgov -L../gov -L/usr/lib -o genesis

clean:
	rm -f genesis

