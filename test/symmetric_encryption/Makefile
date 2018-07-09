all: test_cpp test_java

test_cpp: main.cpp
	g++ main.cpp -I../../.. -L../../gov -lusgov -o test_cpp

test_java: Main.java
    javac ...

run:
    ./do_test

