all: apitool_generated_pairing_*

apitool_generated_pairing_*: apitool data/pairing data/wallet
	./apitool

apitool: main.cpp
	g++ main.cpp -o apitool

clean:
	rm -f apitool
	rm apitool_generated_*

