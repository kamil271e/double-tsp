#!/bin/bash

instances=("kroA100.tsp" "kroB100.tsp")
algos=("nearest" "expansion")

for instance in "${instances[@]}"; do
    for algo in "${algos[@]}"; do
        ./report.sh "$instance" "$algo"
    done
done

python utils/aggregate_results.py
python utils/get_best_visualization.py
