#!/bin/bash

read -r priv_a public_a <<< $(LD_LIBRARY_PATH=../../gov:/usr/lib ../helpers/generate_keys_cpp) 
read -r priv_b public_b <<< $(LD_LIBRARY_PATH=../../gov:/usr/lib ../helpers/generate_keys_cpp) 

MESSAGE=$1

signed="$(LD_LIBRARY_PATH=../../gov:/usr/lib ./test_cpp $priv_a "sign" "$MESSAGE")"
verified="$(java -cp ".:../../sdk/java/libs/*:../../sdk/java/us-wallet-sdk.jar" Main $public_a "verify" "$MESSAGE" "$signed")"
signed2="$(java -cp ".:../../sdk/java/libs/*:../../sdk/java/us-wallet-sdk.jar" Main $priv_a sign "$MESSAGE")"
verified2="$(LD_LIBRARY_PATH=../../gov:/usr/lib ./test_cpp $public_a "verify" "$MESSAGE" "$signed2")"

if [ "$verified" == "true" ] && [ "$verified2" == "1" ]
then
    echo "PASSED"
else 
    echo "FAILED"
fi



