#!/bin/bash

instances=("kroA100.tsp" "kroB100.tsp")
start=("random" "regret")
movements_type=("inner" "inter")
algos=("0" "1") # greedy - 0 / steepest - 1

visited_combinations=()

#for instance in "${instances[@]}"; do
#    for algo in "${algos[@]}"; do
#        for start in "${start[@]}"; do
#            for movement in "${movements_type[@]}"; do
#                combination="$instance$start$movement$algo"
#                if [[ ! " ${visited_combinations[@]} " =~ " $combination " ]]; then
#                    visited_combinations+=("$combination")
#                    # echo "Running local search: $instance $start $movement $algo"
#                    # ./report.sh "$instance" "local" "$start" "$movement" "$algo"
#                fi
#            done
#        done
#    done
#done


regret_random=("regret" "random")
inner_inter=("inner" "inter")
zero_one=("0" "1")
kro=("kroA100" "kroB100")

# Loop through all combinations
for r in "${regret_random[@]}"; do
    for i in "${inner_inter[@]}"; do
        for z in "${zero_one[@]}"; do
            for k in "${kro[@]}"; do
                # Print the combination
                echo "${r} ${i} ${z} ${k}"
            done
        done
    done
done