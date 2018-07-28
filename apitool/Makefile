all: apitool_generated_pairing_*

apitool_generated_pairing_*: apitool data/pairing/cpp data/wallet/cpp data/wallet/tx_make_p2pkh_input.h
	./apitool

apitool: main.cpp
	g++ main.cpp -o apitool

clean:
	rm -f apitool
	rm apitool_generated_*

