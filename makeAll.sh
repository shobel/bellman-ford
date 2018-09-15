#!/bin/bash

echo "Compiling BellmanFord.cpp"
g++ BellmanFord.cpp -o BellmanFord

echo "Compiling BellmanFordPipe.cpp"
g++ BellmanFordPipe.cpp -o BellmanFordPipe

echo "Compiling GraphGen.cpp"
g++ GraphGen.cpp -o GraphGen

echo "Compiling GraphGenPipe.cpp"
g++ GraphGenPipe.cpp -o GraphGenPipe

echo "Compilations complete"
