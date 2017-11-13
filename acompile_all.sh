#!/bin/bash


AOCFLAGS="--profile -v --no-interleaving default"


array=( "aes_swi_big" "aes_swi_medium" "aes_swi_small" "aes_swi_tiny" "des_swi_default" )
for i in "${array[@]}"
do
    echo "Compiling kernel kernel $i"
    aoc $AOCFLAGS "src_cl/$i.cl" -o "bin_cl/$i.aocx"
done
