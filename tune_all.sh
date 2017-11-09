#!/bin/bash


array=( "big" "medium" "small" "tiny" )
for i in "${array[@]}"
do
    echo "Tuning kernel aes_$i"
    ./set_kernel.py aes $1
    ./opencl_ciphers_test tuning
    mv autotune.txt autotune.txt.$1.txt
done
