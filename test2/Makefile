GOVUNITS := $(wildcard */*.o) $(wildcard .o)
CPPS := $(wildcard *.cpp)

CXXFLAGS = -fPIC -g -O0 -DDEBUG -std=c++17

.PHONY: all
all: unit_tests

unit_tests:deps
	g++ ${CXXFLAGS} ${CPPS} ${GOVUNITS} -pthread -lsecp256k1 -lcrypto++ -lusgov -lgtest -lgtest_main -L/usr/lib -L../../../us/gov -o unit_tests

clean:deps-clean
	rm -f unit_tests

deps:
	@for a in $$(ls); do \
	if [ -d $$a ]; then \
	rm -f $$a/$$a.o; \
	$(MAKE) -C $$a; \
	fi; \
	done;

deps-clean:
	@for a in $$(ls); do \
	if [ -d $$a ]; then \
	rm -f $$a/$$a.o; \
	$(MAKE) clean -C $$a; \
	fi; \
	done;

.PHONY: deps

