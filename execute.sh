#!/bin/bash

g++ -std=c++20 -lpthread knightAndFiend.cpp -o knightAndFiend

echo "compiled, executing.."

./knightAndFiend
