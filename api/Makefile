all: apitool_generated__*

apitool_generated__*: apitool data/pairing data/wallet
	./apitool

apitool: main.cpp
	g++ -std=c++17 main.cpp -o apitool 
clean:
	rm -f apitool
	rm -f apitool_generated__*

