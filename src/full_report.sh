#!/bin/bash

instances=("kroA100.tsp" "kroB100.tsp")
algos=("nearest" "expansion")

for instance in "${instances[@]}"; do
    for algo in "${algos[@]}"; do
        ./report.sh "$instance" "$algo"
    done
done
