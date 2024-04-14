#!/bin/bash

declare -a ALGO_TYPES=("nearest" "expansion" "regret" "search_candidates" "search_memory")

if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <instance_path> <algotype>"
    exit 1
fi

instance_path="$1"
algotype="$2"


if [[ ! " ${ALGO_TYPES[@]} " =~ " $algotype " ]]; then
    echo "Invalid algorithm type. Supported types: ${ALGO_TYPES[@]}"
    exit 1
fi

#Check if input data is valid
if [[ ! " ${INPUT_DATA[@]} " =~ " $input_data " ]]; then
    echo "Invalid input data. Supported types: ${INPUT_DATA[@]}"
    exit 1
fi


# N: no. of iterations
N=1
cycles_len_file="../cycles/L_${algotype}_${instance_path::-4}.txt"
cycles_file="../cycles/${algotype}_${instance_path::-4}.txt"

g++ -o main.out main.cpp matrix.cpp tsp.cpp greedy.cpp local_search.cpp local_optim.cpp utils.cpp

for ((i=1; i<=$N; i++)); do
    cpp_output=$(./main.out "$instance_path" "$algotype" )
    echo "$cpp_output"
#    echo "$cpp_output" >> "$cycles_file"
#    python utils/cycle_lengths.py "$instance_path" "$cpp_output" >> "$cycles_len_file"
done

rm main.out
