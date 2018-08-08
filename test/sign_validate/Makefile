CPPS := $(wildcard *.cpp)
HS := $(wildcard *.h)
INC := ../..
CXXFLAGS = -O0 -g -std=c++17

all: test_cpp test_java

test_cpp:
	g++ ${CXXFLAGS} -I${INC} ${CPPS} -pthread -lcrypto++ -I../../.. -L../helpers/generate_keys_cpp.o -L../../gov -L/usr/lib -lusgov -o test_cpp.o

clean:
	find . -name "*.o" -delete
	find . -name "*.class" -delete

test_java: Main.java
	javac -cp ".:../../sdk/java/libs/*:../../sdk/java/us-wallet-sdk.jar" Main.java

run:
	@./do_sign_verify_test.sh

