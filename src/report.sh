#!/bin/bash

declare -a ALGO_TYPES=("nearest" "expansion" "regret" "local" "random_walk" "msls" "ils1" "ils2" "hea")
declare -a MOVEMENTS_TYPES=("edge" "vertex")
declare -a STEEPEST=("0" "1") # greedy - 0 / steepest - 1
declare -a INPUT_DATA=("random" "regret")
declare -a LOCAL_SEARCH=("0" "1")


if [ "$#" -lt 5 ]; then
    echo "Usage: $0 <instance_path> <algotype> <input_data> <movements_type> <steepest> <local_search(optional)>" 
    exit 1
fi

instance_path="$1"
algotype="$2"
input_data="$3"
movements_type="$4"
steepest="$5"

if [ "$#" -eq 6 ]; then
    local_search="$6"
else
    local_search=""
fi

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

_local_search_using="" 
if [[ "$local_search" == "1" ]]; then
    _local_search_using="local"
elif [[ "$local_search" == "0" ]]; then
    _local_search_using="nolocal"
fi


# N: no. of iterations
N=1
if [[ "$local_search" != "" ]]; then
    cycles_len_file="../cycles/L_${algotype}_${input_data}_${movements_type}_${_steepest}_${_local_search_using}_${instance_path::-4}.txt"
    cycles_file="../cycles/${algotype}_${input_data}_${movements_type}_${_steepest}_${_local_search_using}_${instance_path::-4}.txt"
else
    cycles_len_file="../cycles/L_${algotype}_${input_data}_${movements_type}_${_steepest}_${instance_path::-4}.txt"
    cycles_file="../cycles/${algotype}_${input_data}_${movements_type}_${_steepest}_${instance_path::-4}.txt"
fi
   
g++ -o main.out main.cpp matrix.cpp tsp.cpp greedy.cpp local_search.cpp local_search_ext.cpp hea.cpp

for ((i=1; i<=$N; i++)); do
    cpp_output=$(./main.out "$instance_path" "$algotype" "$input_data" "$movements_type" "$steepest" "$local_search")
    echo "$cpp_output" >> "$cycles_file"
    python utils/cycle_lengths.py "$instance_path" "$cpp_output" >> "$cycles_len_file"
done

rm main.out