all: generate_keys_cpp test_cpp test_java

generate_keys_cpp: ../helpers/generate_keys.cpp
	g++ ../helpers/generate_keys.cpp -I../../.. -L../../gov -lusgov -L/usr/local/lib -L/usr/lib -o ../helpers/generate_keys_cpp

test_cpp: generate_keys_cpp main.cpp
	g++ main.cpp -I../../.. -L../../gov -lcrypto++ -lsecp256k1 -L/usr/local/lib -L/usr/lib -lusgov -o test_cpp #removelibsecp

clean:
	find . -name "*.o" -delete
	
test_java: Main.java
	javac -cp ".:../../sdk/java/libs/*:../../sdk/java/us-sdk.jar" Main.java

run:
	./do_sign_verify_test

