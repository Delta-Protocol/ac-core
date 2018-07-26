STDFLAGS:=-std=c++17
PREFIX:=/usr/local
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

cryptos: export CXXFLAGS:=${RELEASEFLAGS}
cryptos: gov/libusgov.so wallet/libuswallet.so govx/us-gov

all: gov/libusgov.so wallet/libuswallet.so govx/us-gov wallet/us-wallet

wallet-debug: export CXXFLAGS:=${DEBUGFLAGS}
wallet-debug: wallet

wallet-release: export CXXFLAGS:=${RELEASEFLAGS}
wallet-release: wallet

gov: govx/us-gov
wallet: walletx/us-wallet

install: release
	install gov/libusgov.so ${PREFIX}/lib
	install wallet/libuswallet.so ${PREFIX}/lib
	install govx/us-gov ${PREFIX}/bin
	install walletx/us-wallet ${PREFIX}/bin
	install etc/init.d/us-wallet /etc/init.d/
	install etc/init.d/us-gov /etc/init.d/
	ldconfig
	systemctl daemon-reload

cryptos-install: cryptos
	install gov/libusgov.so ${PREFIX}/lib
	install wallet/libuswallet.so ${PREFIX}/lib
	install govx/us-gov ${PREFIX}/bin
	install cryptos/etc/init.d/us-wallet /etc/init.d/
	cryptos/etc/cryptos/init.d/us-gov /etc/init.d/
	ldconfig

install-nginx:
	install etc/nginx/sites_available/us-wallet-api.conf /etc/nginx/sites_available
	install etc/nginx/snippets/snakeoil.conf /etc/nginx/snippets/
	install etc/ssl/certs/ssl-cert-snakeoil.pem /etc/ssl/certs/
	install etc/ssl/private/ssl-cert-snakeoil.key /etc/ssl/private/
	install var/www/html/index.html /var/www/html/

gov/libusgov.so:
	$(MAKE) CXXFLAGS="${CXXFLAGS} -fPIC" -C gov;

govx/us-gov: gov/libusgov.so
	$(MAKE) CXXFLAGS="${CXXFLAGS}" -C govx;


wallet/libuswallet.so: gov/libusgov.so
	$(MAKE) CXXFLAGS="${CXXFLAGS} -fPIC" -C wallet;

walletx/us-wallet: wallet/libuswallet.so
	$(MAKE) CXXFLAGS="${CXXFLAGS}" -C walletx ;

.PHONY: all
.PHONY: wallet
.PHONY: gov
.PHONY: debug
.PHONY: release

clean:
	$(MAKE) clean -C gov; \
	$(MAKE) clean -C govx; \
	$(MAKE) clean -C wallet; \
	$(MAKE) clean -C walletx;
