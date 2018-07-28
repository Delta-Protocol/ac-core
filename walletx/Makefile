OBJS :=  json/json.o
CPPS:=$(wildcard *.cpp)
HS:=$(wildcard *.h)
LIBS:=-lusgov -luswallet -ljsoncpp
FCGILIBS:=-lfastcgipp 
CXXFLAGS = -g -O0 -std=c++17 -DDEBUG

.PHONY: all
all: us-wallet

us-wallet: ${CPPS} ${HS} deps
ifeq ($(FCGI),1)
	g++ -DFCGI ${CXXFLAGS} ${CPPS} ${OBJS} -pthread ${LIBS} ${FCGILIBS} -L../gov -L../wallet -I../.. -o us-wallet
else
	g++ ${CXXFLAGS} ${CPPS} ${OBJS} -pthread ${LIBS} -L../gov -L../wallet -I../.. -o us-wallet
endif

clean:
	rm -f us-wallet


deps:
	@for a in $$(ls); do \
	if [ -d $$a ]; then \
    	rm -f $$a/$$a.o; \
	$(MAKE) -C $$a; \
	fi; \
	done;

.PHONY: deps


