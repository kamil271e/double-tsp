#!/bin/bash

regret_random=("regret" "random")
inner_inter=("inner" "inter")
zero_one=("0" "1")
kro=("kroA100.tsp" "kroB100.tsp")

for r in "${regret_random[@]}"; do
    for i in "${inner_inter[@]}"; do
        for z in "${zero_one[@]}"; do
            for k in "${kro[@]}"; do
                echo "${r} ${i} ${z} ${k}"
                ./report.sh "$k" "local" "$r" "$i" "$z"
            done
        done
    done
done

python utils/aggregate_results.py "local"
python utils/generate_visualizations.py "local"