#!/bin/bash

regret_random=$1
edge_vertex=$2
zero_one=$3 # steepest - 1 / greedy - 0
kro=("kroA200.tsp" "kroB200.tsp")
local_search=(0 1) # no local search - 0 / local search - 1


for k in "${kro[@]}"; do
    echo "regret ${k}"
    ./report.sh "$k" "regret" "$regret_random" "$edge_vertex" "$zero_one"
    echo "msls ${k}"
    ./report.sh "$k" "msls" "$regret_random" "$edge_vertex" "$zero_one" 
    echo "ils1 ${k}"
    ./report.sh "$k" "ils1" "$regret_random" "$edge_vertex" "$zero_one" 
    for local in "${local_search[@]}"; do
        echo "ils2 ${k} ${local}"
        ./report.sh "$k" "ils2" "$regret_random" "$edge_vertex" "$zero_one" "$local"
        echo "hea ${k} ${local}"
        ./report.sh "$k" "hea" "$regret_random" "$edge_vertex" "$zero_one" "$local"
    done
    
done

python utils/aggregate_results.py "hea"
python utils/generate_visualizations.py "hea"