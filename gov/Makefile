#https://gist.github.com/xuhdev/1873316
OBJS :=  crypto/crypto.o net/net.o socket/socket.o peer/peer.o dfs/dfs.o blockchain/blockchain.o auth/auth.o cash/cash.o rep/rep.o loan/loan.o
CXXFLAGS := -fPIC ${CXXFLAGS}

.PHONY: all
all: libusgov.so

signal_handler.o: signal_handler.cpp signal_handler.h
	g++ ${CXXFLAGS} -I../.. -c signal_handler.cpp


libusgov.so: deps signal_handler.o
#	g++ -shared -O0 -g -std=c++17 -pthread -lsecp256k1 -I../.. ${OBJS} signal_handler.o -o libusgov.so
	g++ -shared -O0 -g -std=c++17 -pthread -lcryptopp -lsecp256k1 -I../.. -I../../local ${OBJS} signal_handler.o -o libusgov.so
#	g++ -O0 -g -std=c++17 -pthread -lcryptopp -lgmp -lgmpxx -lsecp256k1 -I../..${OBJS} main.cpp signal_handler.cpp  -o bitcoin++


#wallet: wallet.cpp deps
#	g++ -O0 -g -std=c++17 -pthread -lcryptopp -lgmp -lgmpxx -lsecp256k1 -I../.. -I../..wallet.cpp signal_handler.cpp ${OBJS} -o wallet

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

