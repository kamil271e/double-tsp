#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <file_path>"
    exit 1
fi

file_path="$1"

g++ -o main.out main.cpp matrix.cpp tsp.cpp

cpp_output=$(./main.out "$file_path")

python utils/generate_visualization.py "$file_path" "$cpp_output" 

python utils/cycle_lengths.py "$file_path" "$cpp_output" 

rm main.out
