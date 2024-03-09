#!/bin/bash

declare -a ALGO_TYPES=("nearest" "expansion" "regret")

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <file_path> <algotype>"
    exit 1
fi

file_path="$1"
algotype="$2"

if [[ ! " ${ALGO_TYPES[@]} " =~ " $algotype " ]]; then
    echo "Invalid algorithm type. Supported types: ${ALGO_TYPES[@]}"
    exit 1
fi

g++ -o main.out main.cpp matrix.cpp tsp.cpp

N=100
output_file="../cycles/${algotype}_${file_path::-4}.txt"

for ((i=1; i<=$N; i++)); do
    cpp_output=$(./main.out "$file_path" "$algotype")
    python utils/cycle_lengths.py "$file_path" "$cpp_output" >> "$output_file"
    python utils/generate_visualization.py "$file_path" "$algotype"_"${file_path::-4}"_"$i" "$cpp_output" 
done

# script that 

rm main.out
