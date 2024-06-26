#!/bin/bash

instances=("kroA100.tsp" "kroB100.tsp")
algos=("nearest" "expansion" "regret")

for instance in "${instances[@]}"; do
    for algo in "${algos[@]}"; do
        ./report.sh "$instance" "$algo"
    done
done

python utils/aggregate_results.py "greedy"
python utils/generate_visualizations.py "greedy"
