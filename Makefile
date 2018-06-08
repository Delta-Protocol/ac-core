STDFLAGS:=-std=c++17

DEBUGFLAGS:=-ggdb -O0 ${STDFLAGS} -DDEBUG
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

all: gov/libusgov.so wallet/libuswallet.so govd/us-govd walletd/us-walletd

walletd-debug: export CXXFLAGS:=${DEBUGFLAGS}
walletd-debug: walletd

walletd-release: export CXXFLAGS:=${RELEASEFLAGS}
walletd-release: walletd

govd: govd/us-govd
walletd: walletd/us-walletd

gov/libusgov.so:
	$(MAKE) CXXFLAGS="${CXXFLAGS} -fPIC" -C gov;

govd/us-govd: gov/libusgov.so
	$(MAKE) CXXFLAGS="${CXXFLAGS}" -C govd;


wallet/libuswallet.so: gov/libusgov.so
	$(MAKE) CXXFLAGS="${CXXFLAGS} -fPIC" -C wallet;

walletd/us-walletd: wallet/libuswallet.so
	$(MAKE) CXXFLAGS="${CXXFLAGS}" -C walletd ;

.PHONY: all
.PHONY: walletd
.PHONY: govd
.PHONY: debug
.PHONY: release

clean:
	$(MAKE) clean -C gov; \
	$(MAKE) clean -C govd; \
	$(MAKE) clean -C wallet; \
	$(MAKE) clean -C walletd;
