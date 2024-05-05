#!/bin/bash

regret_random=$1
edge_vertex=$2
zero_one=$3 # steepest - 1 / greedy - 0
kro=("kroA200.tsp" "kroB200.tsp")

for k in "${kro[@]}"; do
    echo "${regret_random} ${edge_vertex} ${zero_one} ${k}"
    ./report.sh "$k" "msls" "$regret_random" "$edge_vertex" "$zero_one"
    ./report.sh "$k" "ils1" "$regret_random" "$edge_vertex" "$zero_one"
    ./report.sh "$k" "ils2" "$regret_random" "$edge_vertex" "$zero_one"
done

python utils/aggregate_results.py "local_ext"
#python utils/generate_visualizations.py "local_ext"