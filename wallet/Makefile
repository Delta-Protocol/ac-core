OBJS :=  json/json.o
CPPS := $(wildcard *.cpp)
HS := $(wildcard *.h)

CXXFLAGS = -fPIC -g -O0 -DDEBUG -std=c++17

.PHONY: all
all: libuswallet.so

libuswallet.so: ${CPPS} ${HS} deps
	g++ ${CXXFLAGS} -shared ${CPPS} ${OBJS} -pthread -lsecp256k1 -lusgov -L../gov -I../.. -o libuswallet.so

clean:
	rm -f libuswallet.so

deps:
	@for a in $$(ls); do \
	if [ -d $$a ]; then \
	rm -f $$a/$$a.o; \
	$(MAKE) -C $$a; \
	fi; \
	done;

.PHONY: deps

