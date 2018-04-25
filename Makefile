STDFLAGS:=-std=c++17

DEBUGFLAGS:=-g -O0 ${STDFLAGS} -DDEBUG
RELEASEFLAGS:=-O3 ${STDFLAGS}


#dev
CXXFLAGS:=-g -O0 -DDEBUG ${STDFLAGS}
#distr
#CXXFLAGS:=${STDFLAGS}                  

export CXXFLAGS

debug: export CXXFLAGS:=${DEBUGFLAGS}
debug: all

release: export CXXFLAGS:=${RELEASEFLAGS}
release: all

all: gov/libusgov.so govd/us-govd wallet/us-wallet

govd: govd/us-govd

wallet-debug: export CXXFLAGS:=${DEBUGFLAGS}
wallet-debug: wallet

wallet-release: export CXXFLAGS:=${RELEASEFLAGS}
wallet-release: wallet
wallet: wallet/us-wallet

gov/libusgov.so:
	$(MAKE) CXXFLAGS="${CXXFLAGS} -fPIC" -C gov;

govd/us-govd:
	$(MAKE) CXXFLAGS="${CXXFLAGS}" -C govd;

wallet/us-wallet:
	$(MAKE) CXXFLAGS="${CXXFLAGS}" -C wallet ;

.PHONY: all
.PHONY: wallet
.PHONY: govd
.PHONY: debug
.PHONY: release

clean:
	$(MAKE) clean -C gov; \
	$(MAKE) clean -C govd; \
	$(MAKE) clean -C wallet;
