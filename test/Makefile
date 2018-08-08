CPPS := $(wildcard *.cpp)
HS := $(wildcard *.h)
INC := ../..
CXXFLAGS = -O0 -g -std=c++17

.PHONY: all

all: unit_tests
	cd symmetric_encryption; make; cd ..
	cd sign_validate; make; cd ..

unit_tests: ${HS} ${CPPS}
	g++ ${CXXFLAGS} -I${INC} ${CPPS} -pthread -lsecp256k1 -lcrypto++ -lusgov -L/usr/lib -L../gov -o unit_tests.o

clean:
	rm -f unit_tests
	find . -name "*.o" -delete
	cd symmetric_encryption; make clean; cd ..
	cd sign_validate; make clean; cd ..


