#!/bin/bash

g++ -O2 -fopenmp lontano.cpp -o lontano

echo "example.in"
time ./lontano < dataset/example.in > dataset/example.out
echo 

echo "small.in"
time ./lontano < dataset/small.in   > dataset/small.out
echo 

echo "medium.in"
time ./lontano < dataset/medium.in  > dataset/medium.out
echo 

echo "big.in"
time ./lontano < dataset/big.in     > dataset/big.out
