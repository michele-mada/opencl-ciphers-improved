#!/bin/bash


array=( "big" "medium" "small" "tiny" )
for i in "${array[@]}"
do
    echo "Tuning kernel aes_$i"
    ./set_kernel.py aes $i
    ./opencl_ciphers_test tuning
    mv autotune.txt autotune.txt.$i.txt
    mv profile.mon profile.mon.$i.mon
done
