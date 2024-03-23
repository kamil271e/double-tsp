#!/bin/bash

instances=("kroA100.tsp" "kroB100.tsp")
start=("random" "regret")
movements_type=("inner" "inter")
algos=("greedy" "steepest")

visited_combinations=()

for instance in "${instances[@]}"; do
    for algo in "${algos[@]}"; do
        for start in "${start[@]}"; do
            for movement in "${movements_type[@]}"; do
                combination="$instance$start$movement$algo"
                if [[ ! " ${visited_combinations[@]} " =~ " $combination " ]]; then
                    visited_combinations+=("$combination")
                    echo "Running local search: $instance $start $movement $algo"
                    ./report.sh "$instance" "local" "$start" "$movement" "$algo"
                fi
            done
        done
    done
done