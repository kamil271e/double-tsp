#!/bin/bash

declare -a ALGO_TYPES=("nearest" "expansion" "regret" "local" "random_walk" "search_candidates" "search_memory")
declare -a MOVEMENTS_TYPES=("edge" "vertex")
declare -a STEEPEST=("0" "1") # greedy - 0 / steepest - 1
declare -a INPUT_DATA=("random" "regret")


if [ "$#" -ne 5 ]; then
    echo "Usage: $0 <instance_path> <algotype> <input_data> <movements_type> <steepest>" 
    exit 1
fi

instance_path="$1"
algotype="$2"
input_data="$3"
movements_type="$4"
steepest="$5"

if [[ ! " ${ALGO_TYPES[@]} " =~ " $algotype " ]]; then
    echo "Invalid algorithm type. Supported types: ${ALGO_TYPES[@]}"
    exit 1
fi

#Check if input data is valid
if [[ ! " ${INPUT_DATA[@]} " =~ " $input_data " ]]; then
    echo "Invalid input data. Supported types: ${INPUT_DATA[@]}"
    exit 1
fi

#Check if movements type is valid
if [[ ! " ${MOVEMENTS_TYPES[@]} " =~ " $movements_type " ]]; then
    echo "Invalid movements type. Supported types: ${MOVEMENTS_TYPES[@]}"
    exit 1
fi

#Check if steepest is valid
if [[ ! " ${STEEPEST[@]} " =~ " $steepest " ]]; then
    echo "Invalid steepest type. Supported types: ${STEEPEST[@]}"
    exit 1
fi

_steepest=""
if [[ "$steepest" == "1" ]]; then
    _steepest="steepest"
else
    _steepest="greedy"
fi

# N: no. of iterations
N=1
cycles_len_file="../cycles/L_${algotype}_${input_data}_${movements_type}_${_steepest}_${instance_path::-4}.txt"
cycles_file="../cycles/${algotype}_${input_data}_${movements_type}_${_steepest}_${instance_path::-4}.txt"

g++ -o main.out main.cpp matrix.cpp tsp.cpp local_search.cpp greedy.cpp local_optim.cpp utils.cpp

for ((i=1; i<=$N; i++)); do
    cpp_output=$(./main.out "$instance_path" "$algotype" "$input_data" "$movements_type" "$steepest")
    echo "$cpp_output" >> "$cycles_file"
    python utils/cycle_lengths.py "$instance_path" "$cpp_output" >> "$cycles_len_file"
done

rm main.out
