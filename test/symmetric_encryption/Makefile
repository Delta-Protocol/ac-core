all: generate_keys_cpp test_cpp test_java

generate_keys_cpp: ../helpers/generate_keys.cpp
	g++ ../helpers/generate_keys.cpp -I../../.. -L../../gov -lusgov -L/usr/lib -o ../helpers/generate_keys_cpp

test_cpp: generate_keys_cpp main.cpp
	g++ main.cpp -I../../.. -L../../gov -lcrypto++ -L/usr/lib -lusgov -o test_cpp

clean:
	find . -name "*.o" -delete
	find . -name "*.class" -delete
	
test_java: Main.java
	javac -cp ".:../../sdk/java/libs/*:../../sdk/java/us-wallet-sdk.jar" Main.java

run:
	@./do_symmetric_encryption_test

