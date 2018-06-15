#https://gist.github.com/xuhdev/1873316
OBJS :=  crypto/crypto.o net/net.o socket/socket.o peer/peer.o dfs/dfs.o blockchain/blockchain.o auth/auth.o cash/cash.o rep/rep.o loan/loan.o nova/nova.o
CXXFLAGS := -fPIC ${CXXFLAGS}

.PHONY: all
all: libusgov.so

signal_handler.o: signal_handler.cpp signal_handler.h
	g++ ${CXXFLAGS} -I../.. -c signal_handler.cpp

cfg.o: cfg.cpp cfg.h
	g++ ${CXXFLAGS} -I../.. -c cfg.cpp

libusgov.so: deps signal_handler.o cfg.o
	g++ -shared -O0 -g -std=c++17 -pthread -lcryptopp -lsecp256k1 -I../.. ${OBJS} signal_handler.o cfg.o -o libusgov.so

deps:
	@for a in $$(ls); do \
	if [ -d $$a ]; then \
	echo "processing folder $$a"; \
        echo "deleting $$a/$$a.o"; \
        rm -f $$a/$$a.o; \
	$(MAKE) -C $$a; \
	fi; \
	done;

.PHONY: deps

clean:
	find . -name "*.o" -delete
	rm -f libusgov.so

