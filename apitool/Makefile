all: apitool_generated__*

apitool_generated__*: apitool data/pairing data/wallet
	./apitool

apitool: main.cpp
	g++ main.cpp -o apitool

clean:
	rm -f apitool
	rm apitool_generated__*

